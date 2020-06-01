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
                        SENTRY_URL = 'http://zonme.to2x.ovh:9000'
                        SENTRY_ORG = 'lvwl'
                        SENTRY_PROJECT = 'understory'
                    }
                    steps {
                        sh 'cmake -GNinja -B_genWindows -H. -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/windows-ci.cmake'
                        sh 'cmake --build ./_genWindows --target zipForDeploy'
                        withCredentials([string(credentialsId: 'jenkins-bintray-api-key', variable: 'BINTRAY_API_KEY')]) {
                            sh 'curl -T _genWindows/installer.zip   -uamphaal:$BINTRAY_API_KEY -H "X-Bintray-Package: install-packages" -H "X-Bintray-Version: latest" -H "X-Bintray-Publish: 1" -H "X-Bintray-Override: 1" -H "X-Bintray-Explode: 1" https://api.bintray.com/content/amphaal/rpgrpz/'
                            sh 'curl -T _genWindows/repository.zip  -uamphaal:$BINTRAY_API_KEY -H "X-Bintray-Package: ifw"              -H "X-Bintray-Version: latest" -H "X-Bintray-Publish: 1" -H "X-Bintray-Override: 1" -H "X-Bintray-Explode: 1" https://api.bintray.com/content/amphaal/rpgrpz/ifw-win64/'
                        }
                    }
                }
            }
        }
    }
}