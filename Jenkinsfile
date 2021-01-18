pipeline {
    agent none
    stages {
        stage('Configure && Build') {
            parallel {
                stage('MinGW') {
                    agent {
                        docker { image 'amphaal/understory-ci-windows' }
                    }
                    environment {
                        SENTRY_URL = 'https://sentry.io/'
                        SENTRY_ORG = 'lvwl'
                        SENTRY_PROJECT = 'understory'
                    }
                    steps {
                        sh 'git submodule update --init'
                        sh 'cmake -GNinja -B_pkg -H. -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/windows-ci.cmake'
                        sh 'ninja -C_pkg zipForDeploy'
                        withCredentials([string(credentialsId: 'jenkins-bintray-api-key', variable: 'BINTRAY_API_KEY')]) {
                            sh 'curl -T _pkg/installer.zip   -uamphaal:$BINTRAY_API_KEY -H "X-Bintray-Package: install-packages" -H "X-Bintray-Version: latest" -H "X-Bintray-Publish: 1" -H "X-Bintray-Override: 1" -H "X-Bintray-Explode: 1" https://api.bintray.com/content/amphaal/understory/'
                            sh 'curl -T _pkg/repository.zip  -uamphaal:$BINTRAY_API_KEY -H "X-Bintray-Package: repository"       -H "X-Bintray-Version: latest" -H "X-Bintray-Publish: 1" -H "X-Bintray-Override: 1" -H "X-Bintray-Explode: 1" https://api.bintray.com/content/amphaal/understory/repository/'
                        }
                    }
                }
            }
        }
    }
}