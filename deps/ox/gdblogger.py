
import json
import os
import socket

log_server = None

class LogFunction:
    def __init__(self, f):
        self.function = f['function']
        self.ignore_frames = f['ignore_frames']
        self.file_var = f['file_var']
        self.line_var = f['line_var']
        self.channel_var = f['channel_var']
        self.msg_var = f['msg_var']

class Config:
    def __init__(self, c):
        self.log_functions = {}
        for f in c['log_functions']:
            lf = LogFunction(f)
            self.log_functions[lf.function] = lf

class Msg(dict):
    def __init__(self, type, data):
        dict.__init__(self, type=type, data=data)

class InitMsg(dict):
    def __init__(self, cmd):
        dict.__init__(self, cmd=cmd)

class Field(dict):
    name = ''
    value = None
    type = ''
    fields = None
    def __init__(self, name, value, field_type):
        self.name = name
        if field_type == gdb.TYPE_CODE_STRUCT:
            self.type = 'struct'
            self.fields = value
        elif field_type == gdb.TYPE_CODE_UNION:
            self.type = 'union'
            self.value = value
        elif field_type == gdb.TYPE_CODE_ENUM:
            self.type = 'enum'
            self.value = value
        elif field_type == gdb.TYPE_CODE_ARRAY:
            self.type = 'array'
            self.fields = value
        else:
            self.type = 'scalar'
            self.value = value
        dict.__init__(self, name=name, value=self.value, fields=self.fields, type=self.type)

class Frame(dict):
    function = ''
    file = ''
    line = 0
    fields = []
    def __init__(self, function, file, line, fields):
        dict.__init__(self, function=function, file=file, \
                      line=line, fields=fields)

class TraceEvent(dict):
    def __init__(self, channel, log_msg, frames):
        dict.__init__(self, channel=channel, log_msg=log_msg, frames=frames)

class Executor:
    def __init__(self, cmd):
        self.__cmd = cmd

    def __call__(self):
        gdb.execute(self.__cmd)

def has_fields(t):
    return t.code == gdb.TYPE_CODE_STRUCT or t.code == gdb.TYPE_CODE_UNION \
           or t.code == gdb.TYPE_CODE_ENUM

def build_value(value):
    t = value.type
    if t is not None:
        if has_fields(t) and not value.is_optimized_out:
            fields = []
            if t.keys() is not None:
                for key in t.keys():
                    # put this in a try to handle builtin types that have some weird keys
                    try:
                        val = value[key]
                        fields.append(Field(key, build_value(val), val.type.code))
                    except gdb.error:
                        fields.append(Field('invalid_key', '', 0))
                        pass
            return fields
        elif t.code == gdb.TYPE_CODE_ARRAY:
            fields = []
            if t.fields() is not None:
                r = t.range()
                for i in range(r[0], r[1] + 1):
                    fields.append(Field(i, build_value(value[i]), value[i].type.code))
            return fields
        else:
            # parse as string
            return str(value)

def frame_source_line(bt, level):
    f = bt[level].split(' ')
    source_line = f[len(f)-1].split(':')
    file = source_line[0]
    line = int(source_line[1])
    return (file, line)

def handle_tracepoint(event):
    global log_server
    gdb.execute('set scheduler-locking on')
    # iterate over frames
    gdb.newest_frame()
    cur_frame = gdb.selected_frame()
    if cur_frame.name() in config.log_functions:
        func_config = config.log_functions[cur_frame.name()]
        #print('Frames')
        frame_no = 0
        frames = []
        log_msg = ''
        channel = ''
        bt = gdb.execute('bt', to_string=True).split('\n')
        bt.pop()
        # scrape channel and message values from top frame
        cur_frame.select()
        block = cur_frame.block()
        for symbol in block:
            field_name = symbol.name
            if symbol.is_argument or symbol.is_variable:
                #print(help(symbol.symtab))
                sym_val = symbol.value(cur_frame)
                if field_name == func_config.channel_var:
                    channel = sym_val.string()
                elif field_name == func_config.msg_var:
                    log_msg = sym_val.string()
        # skip ignored frames
        for i in range(0, func_config.ignore_frames):
            cur_frame = cur_frame.older()
            bt.pop(0)
        while cur_frame is not None:
            cur_frame.select()
            function = cur_frame.name()
            source_line = frame_source_line(bt, frame_no)
            file_name = source_line[0]
            line_number = source_line[1]
            fields = []
            frame_symbols = set()
            #print('{}: {}'.format(frame_no, cur_frame.name()))
            #print('\tArchitecture: ' + cur_frame.architecture().name())
            # iterate over blocks
            #print('\tVariables:')
            #block = cur_frame.block()
            #while block is not None:
            #    # iterate over symbols
            #    for symbol in block:
            #        field_name = symbol.name
            #        if (symbol.is_argument or symbol.is_variable) \
            #           and not field_name in frame_symbols and symbol.line < line_number:
            #            #print(help(symbol.symtab))
            #            sym_val = symbol.value(cur_frame)
            #            field_value = build_value(sym_val)
            #            fields.append(Field(field_name, field_value, sym_val.type.code))
            #            #print('\t\t{}: {}'.format(field_name, json.dumps(field_value)))
            #            frame_symbols.add(field_name)
            #    # end: iterate over symbols
            #    block = block.superblock
            # end: iterate over blocks
            cur_frame = cur_frame.older()
            frames.append(Frame(function, file_name, line_number, fields))
            frame_no += 1
        # end: iterate over frames
        msg = json.dumps(Msg('TraceEvent', TraceEvent(channel, log_msg, frames)))
        if log_server is not None:
            log_server.send(bytes(msg, 'utf-8'))
    gdb.execute('set scheduler-locking off')
    gdb.post_event(Executor("continue"))

def handle_exit(event):
    if log_server is not None:
        log_server.close()
    gdb.post_event(Executor('quit'))

def connect_to_server(ip_port):
    ip_port = ip_port.split(':')
    ip_port = (ip_port[0], int(ip_port[1]))
    global log_server
    log_server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        log_server.connect(ip_port)
    except ConnectionRefusedError:
        print("Could not connect to log server.")
        log_server = None

ip_port = os.environ.get('LOG_SERVER')
if ip_port is not None:
    connect_to_server(ip_port)
else:
    print('WARNING: No log server is configured, discarding logs. Please set LOG_SERVER environment variable, e.g. LOG_SERVER=127.0.0.1:5590')

# load config file
try:
    with open(".gdblogger.json", mode='r', encoding='utf-8') as f:
        global config
        config = f.read()
        config = json.loads(config)
        config = Config(config)
        for lf in config.log_functions:
            gdb.Breakpoint(lf)
except FileNotFoundError:
    pass

gdb.events.stop.connect(handle_tracepoint)
gdb.events.exited.connect(handle_exit)

if log_server is not None:
    log_server.send(bytes(json.dumps(Msg('Init', InitMsg(''))), 'utf-8'))
