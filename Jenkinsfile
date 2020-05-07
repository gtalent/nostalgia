pipeline {
	agent any
	stages {
		stage('Environment') {
			steps {
				sh 'conan profile update settings.compiler.libcxx=libstdc++11 default'
				sh 'conan remote add -f bincrafters https://api.bintray.com/conan/bincrafters/public-conan'
				sh 'make conan'
			}
		}
		stage('Build Asan') {
			steps {
				sh 'make purge configure-debug'
				sh 'make'
			}
		}
		stage('Test Asan') {
			steps {
				sh 'make test'
			}
		}
		stage('Build Debug') {
			steps {
				sh 'make purge configure-debug'
				sh 'make'
			}
		}
		stage('Test Debug') {
			steps {
				sh 'make test'
			}
		}
		stage('Build Release') {
			steps {
				sh 'make purge configure-release'
				sh 'make'
			}
		}
		stage('Test Release') {
			steps {
				sh 'make test'
			}
		}
	}
}
