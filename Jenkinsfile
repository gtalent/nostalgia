pipeline {
	stages {
		stage('Build') {
			steps {
				sh 'make release'
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
