pipeline {
	agent { dockerfile true }
	stages {
		stage('Build') {
			steps {
				sh 'make release'
				sh 'make'
			}
		}
	}
}
