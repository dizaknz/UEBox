pipeline {
    agent any
    stages {
        stage('build') {
            steps {
                script {
                    echo 'Build starting'
                    def statusCode = sh script:"docker buildx build --progress=plain --compress -t uebox:ci .", returnStatus:true
                    if (statusCode == 0) {
                        currentBuild.result = "SUCCESS"
                    } else {
                        currentBuild.result = "FAILURE"
                    }
                }
            }
            post {
                success {
                    echo 'Build succeeded'
                }
                failure {
                    echo 'Build failed'
                }
            }
        }
    }
}
