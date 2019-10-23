pipeline {
	agent any
	stages {
		stage('Build') {
			steps {
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
