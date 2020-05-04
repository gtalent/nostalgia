pipeline {
	agent any
	stages {
		stage('Environment') {
			steps {
				sh 'conan profile update settings.compiler.libcxx=libstdc++11 default'
				sh 'conan remote add -f bincrafters https://api.bintray.com/conan/bincrafters/public-conan'
			}
		}
		stage('Build') {
			steps {
				sh 'make conan'
				sh 'make configure-debug configure-release'
				sh 'make'
			}
		}
		stage('Test') {
			steps {
				sh 'make test'
			}
		}
	}
}
