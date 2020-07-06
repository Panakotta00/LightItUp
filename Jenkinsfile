pipeline {
    agent {
        label 'Windows2019'
    }

    options {
        disableConcurrentBuilds()
        skipDefaultCheckout(true)
    }

	environment {
        SML_BRANCH = 'sml-2.1.3'
    }

    stages {
        stage('Checkout') {
            steps {
                checkout scm: [
                    $class: 'GitSCM',
                    branches: scm.branches,
                    extensions: [[
                        $class: 'RelativeTargetDirectory',
                        relativeTargetDir: 'LightItUp'
                    ]],
                    submoduleCfg: scm.submoduleCfg,
                    doGenerateSubmoduleConfigurations: scm.doGenerateSubmoduleConfigurations,
                    userRemoteConfigs: scm.userRemoteConfigs
                ]
            }
        }

        stage('Setup UE4') {
            steps {
                dir('ue4') {
                    withCredentials([string(credentialsId: 'GitHub-API', variable: 'GITHUB_TOKEN')]) {
                        retry(3) {
                            bat label: '', script: 'github-release download --user SatisfactoryModdingUE --repo UnrealEngine -l -n "UnrealEngine-CSS-Editor-Win64.zip" > UnrealEngine-CSS-Editor-Win64.zip'
                        }
                        bat label: '', script: '7z x UnrealEngine-CSS-Editor-Win64.zip'
                    }
                    bat label: '', script: 'SetupScripts\\Register.bat'
                }
            }
        }
        
        stage('Setup WWise & Rider') {
            steps {
                dir("LightItUp") {
                    bat label: '', script: '7z x %WWISE_PLUGIN% -oPlugins\\'
                }
            }
        }
        
        stage('Setup SML') {
            steps {
                bat label: '', script: 'git clone --branch %SML_BRANCH% https://github.com/satisfactorymodding/SatisfactoryModLoader.git'
                bat label: '', script: 'xcopy /Y /E /I SatisfactoryModLoader\\Source\\FactoryGame LightItUp\\Source\\FactoryGame > copy.log'
                bat label: '', script: 'xcopy /Y /E /I SatisfactoryModLoader\\Source\\SML LightItUp\\Source\\SML > copy.log'
                bat label: '', script: 'xcopy /Y /E /I SatisfactoryModLoader\\Plugins\\Alpakit LightItUp\\Plugins\\Alpakit > copy.log'
                bat label: '', script: 'xcopy /Y /E /I SatisfactoryModLoader\\Content\\SML LightItUp\\Content\\SML > copy.log'
            }
        }

        stage('Build LightItUp') {
            steps {
                bat label: '', script: '.\\ue4\\Engine\\Binaries\\DotNET\\UnrealBuildTool.exe  -projectfiles -project="%WORKSPACE%\\LightItUp\\FactoryGame.uproject" -game -rocket -progress'
                bat label: '', script: 'MSBuild.exe .\\LightItUp\\FactoryGame.sln /p:Configuration="Shipping" /p:Platform="Win64" /t:"Games\\FactoryGame"'
                bat label: '', script: 'MSBuild.exe .\\LightItUp\\FactoryGame.sln /p:Configuration="Development Editor" /p:Platform="Win64" /t:"Games\\FactoryGame"'
                retry(3) {
                    bat label: '', script: '.\\ue4\\Engine\\Build\\BatchFiles\\RunUAT.bat BuildCookRun -nop4 -project="%WORKSPACE%\\LightItUp\\FactoryGame.uproject" -cook -skipstage -iterate'
                }
                
                dir('ToPak/FactoryGame/Content') {
                    bat label: '', script: 'xcopy /Y /E /I %WORKSPACE%\\LightItUp\\Saved\\Cooked\\WindowsNoEditor\\FactoryGame\\Content\\LightItUp .\\LightItUp > copy.log'
                }
                dir('ToPak') {
                    bat label: '', script: 'copy %U4PAK% .'
                    bat label: '', script: 'python .\\u4pak.py pack LightItUp.pak FactoryGame'
                    bat label: '', script: 'rmdir /S /Q .\\FactoryGame'
                }
                dir('ToPak/FactoryGame/Content') {
                    bat label: '', script: 'xcopy /Y /E /I %WORKSPACE%\\LightItUp\\Saved\\Cooked\\WindowsNoEditor\\FactoryGame\\Content\\SML .\\SML > copy.log'
                }
                dir('ToPak') {
                    bat label: '', script: 'copy %U4PAK% .'
                    bat label: '', script: 'python .\\u4pak.py pack SML.pak FactoryGame'
                }
            }
        }

        stage('Package FicsIt-Networks') {
            when {
                not {
                    changeRequest()
                }
            }
            steps {
                dir('FicsIt-Networks') {
                    bat label: '', script: "7z a -tzip -mx9 -mm=LZMA -xr!*.pdb ..\\LightItUp-${BRANCH_NAME}-${BUILD_NUMBER}-Win64.zip Binaries\\ Config\\ Content\\ Library\\ Plugins\\Alpakit\\ Source\\ .gitattributes .gitignore FactoryGame.uproject LICENSE README.md ..\\ToPak"
                    bat label: '', script: 'copy .\\Binaries\\Win64\\UE4-LightItUp-Win64-Shipping.* .\\'
                    bat label: '', script: 'copy ..\\ToPak\\LightItUp.pak .\\'
                    bat label: '', script: "7z a -tzip -mx9 FicsIt-Networks.smod .\\data.json .\\LightItUp.bmp .\\UE4-LightItUp-Win64-Shipping.dll .\\UE4-LightItUp-Win64-Shipping.pdb .\\LightItUp.pak"
                }
                bat label: '', script: '7z x %EXTRACT_AND_GO% -o.\\'
                bat label: '', script: 'copy .\\LightItUp\\Binaries\\Win64\\UE4-SML-Win64-Shipping.* .\\ExtractAndGo\\loaders\\'
                bat label: '', script: 'copy .\\ToPak\\SML.pak .\\ExtractAndGo\\loaders\\'
                bat label: '', script: 'copy .\\LightItUp\\Binaries\\Win64\\UE4-LightItUp-Win64-Shipping.* .\\ExtractAndGo\\mods\\'
                bat label: '', script: 'copy .\\ToPak\\LightItUp.pak .\\ExtractAndGo\\mods\\'
                bat label: '', script: "7z a -tzip -mx9 -mm=LZMA LightItUp-ExtractAndGo-${BRANCH_NAME}-${BUILD_NUMBER}-Win64.zip .\\ExtractAndGo\\*"
                archiveArtifacts artifacts: "LightItUp-${BRANCH_NAME}-${BUILD_NUMBER}-Win64.zip", fingerprint: true, onlyIfSuccessful: true
                archiveArtifacts artifacts: "LightItUp-ExtractAndGo-${BRANCH_NAME}-${BUILD_NUMBER}-Win64.zip", fingerprint: true, onlyIfSuccessful: true
                archiveArtifacts artifacts: 'LightItUp\\Binaries\\Win64\\UE4-LightItUp-Win64-Shipping.dll', fingerprint: true, onlyIfSuccessful: true
                archiveArtifacts artifacts: 'ToPak\\LightItUp.pak', fingerprint: true, onlyIfSuccessful: true
                archiveArtifacts artifacts: 'LightItUp\\LightItUp.smod', fingerprint: true, onlyIfSuccessful: true
            }
        }
    }
    post {
        always {
            cleanWs()
        }
    }
}