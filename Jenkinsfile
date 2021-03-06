#!groovy
pipeline {
	agent {
		node{
			label 'unreal'
		}
	}
	
	parameters {
		booleanParam(name: 'CLEAN_BUILD', defaultValue: true, description: 'Options for doing a clean build when is true')
		string(name: 'REF_BUILD_NUMBER', defaultValue: '', description: 'Specified build number to copy dependencies(E.g.: 22, using latest build when empty)')
		string(name: 'JOB_NAME', defaultValue: 'RUYI-Platform-CleanBuild', description: 'Specified job name to copy dependencies')
		booleanParam(name: 'MAIL_ON_FAILED', defaultValue: true, description: 'Options for sending mail when failed')
	}
	
	options{
		//Set a timeout period for the Pipeline run, after which Jenkins should abort the Pipeline.
		timeout(time: 2, unit: 'HOURS')	
		//Persist artifacts and console output for the specific number of recent Pipeline runs. 
		buildDiscarder(logRotator(numToKeepStr: '30', artifactNumToKeepStr: '30'))
		//Skip checking out code from source control by default in the agent directive.
		skipDefaultCheckout()
		//Disallow concurrent executions of the Pipeline.
		//disableConcurrentBuilds()
	}
	
	environment{
		//Windows command prompt encoding(65001=UTF8;437=US.English;936=GBK)
		WIN_CMD_ENCODING = '65001'
		//Nuget Packages home
		NUGET_PACKAGES = "c:/jenkins/packages"
		//Unreal Engine root
		UE_ROOT = "C:/Program Files/Epic Games/UE_4.18/Engine"
		//Temp folder
		TEMP_DIR = 'temp'
		//Ruyi SDK CPP folder
		RUYI_SDK_CPP = "${TEMP_DIR}\\RuyiSDKCpp"
		//Ruyi DevTools folder
		RUYI_DEV_ROOT = "${TEMP_DIR}\\DevToolsInternal"
		//Unreal Demo Root
		DEMO_PROJECT_ROOT = "unreal_demo"
		//DEMO SDK CPP folder
		DEMO_SDKCPP_ROOT = "${DEMO_PROJECT_ROOT}\\Source\\RuyiSDKDemo"
		//Unreal packed target
		COOKED_ROOT = "${DEMO_PROJECT_ROOT}/Pack"
		//Archive root
		ARCHIVE_ROOT = 'archives'
		//File path for saving commit id
		COMMIT_ID_FILE = "${ARCHIVE_ROOT}\\commit-id"
		//Mail recipient on failed
		MAIL_RECIPIENT = 'sw-engr@playruyi.com,cc:chris.zhang@playruyi.com'
	}
		
	stages {
		stage('Initialize') {
			steps{
				//clean workspace
				script{
					//Never did a clean build in multi-branch pipeline task lol
					if(env.BRANCH_NAME == null && params.CLEAN_BUILD){
						deleteDir()
					}else{
						step([$class: 'WsCleanup', patterns: [[pattern: "**/Binaries/**/**", type: 'INCLUDE'],[pattern: "**/Pack/RuyiSDKDemo/**/**", type: 'INCLUDE'],[pattern: "**/Source/RuyiSDKDemo/lib/**/**", type: 'INCLUDE'],[pattern: "**/Source/RuyiSDKDemo/include/**/**", type: 'INCLUDE'],[pattern: "**/.git/*.lock", type: 'INCLUDE']]])
					}
									
					checkout changelog: false, poll: false, scm: [$class: 'GitSCM', branches:  scm.branches,
						 doGenerateSubmoduleConfigurations: false, extensions: [
							[$class: 'RelativeTargetDirectory', relativeTargetDir: "${DEMO_PROJECT_ROOT}"],
							[$class: 'CleanBeforeCheckout'],
							[$class: 'CheckoutOption', timeout: 60],
							[$class: 'CloneOption', honorRefspec: true, depth: 0, noTags: true, reference: '', shallow: true,timeout: 60]
						 ], 
						 submoduleCfg: [], 
						 userRemoteConfigs: [[credentialsId: scm.userRemoteConfigs[0].credentialsId, url: scm.userRemoteConfigs[0].url]]
					]
				}
			}
			
			post {
				success {
					stage_success env.STAGE_NAME
				}
				failure {
					stage_failed env.STAGE_NAME
				}
			}
		}
		
		stage('Copy dependencies'){
			steps{
				script{
					def jobName = params.JOB_NAME
					def sel = [$class:'StatusBuildSelector',stable:false]
					
					if(params.REF_BUILD_NUMBER?.trim())
						sel = specific("${params.REF_BUILD_NUMBER}")
						
					step([$class:'CopyArtifact',filter:'RuyiSDKCpp/**/*,DevToolsInternal/**/*',target:"${TEMP_DIR}",projectName: "${jobName}",selector: sel])
					
					bat """
						md ${DEMO_SDKCPP_ROOT}\\lib
						xcopy ${RUYI_SDK_CPP}\\lib\\* /s /i /y ${DEMO_SDKCPP_ROOT}\\lib\\*
						md ${DEMO_SDKCPP_ROOT}\\include
						xcopy ${RUYI_SDK_CPP}\\include\\* /s /i /y ${DEMO_SDKCPP_ROOT}\\include\\*
					"""
				}
			}
			
			post {
				success {
					stage_success env.STAGE_NAME
				}
				failure {
					stage_failed env.STAGE_NAME
				}
			}
		}
		
		stage('Build Libs'){
			steps{
				//1.Generate VS project file
				bat """
					chcp ${WIN_CMD_ENCODING} 
					"${UE_ROOT}/Binaries/DotNET/UnrealBuildTool.exe" -projectfiles -project="${workspace}/${DEMO_PROJECT_ROOT}/RuyiSDKDemo.uproject" -CurrentPlatform -2017 -game -rocket -progress
				"""
				withEnv(["PATH+NUGET_PACKAGES=${NUGET_PACKAGES}"]){
					//2.VS Build target - Development & Shipping in Win64 platform
					bat """
						chcp ${WIN_CMD_ENCODING}
						"${tool 'MSBuild'}" "${DEMO_PROJECT_ROOT}\\RuyiSDKDemo.sln" /t:restore;build /m:4 /p:Configuration="Development Editor" /p:Platform="Win64"
						"${tool 'MSBuild'}" "${DEMO_PROJECT_ROOT}\\RuyiSDKDemo.sln" /t:restore;build /m:4 /p:Configuration=Development /p:Platform="Win64"
						"${tool 'MSBuild'}" "${DEMO_PROJECT_ROOT}\\RuyiSDKDemo.sln" /t:restore;build /m:4 /p:Configuration=Shipping /p:Platform="Win64"
					"""
				}
			}
			
			post {
				success {
					stage_success env.STAGE_NAME
				}
				failure {
					stage_failed env.STAGE_NAME
				}
			}
		}
		
		stage('Cook Demo'){
			steps{
				//Cook
				bat """
					chcp ${WIN_CMD_ENCODING}
					del ${DEMO_PROJECT_ROOT}\\Pack.zip /F /Q
					"${UE_ROOT}/Build/BatchFiles/RunUAT.bat" BuildCookRun -project="${workspace}/${DEMO_PROJECT_ROOT}/RuyiSDKDemo.uproject" -noP4 -platform=Win64 -clientconfig=Development -serverconfig=Development -cook -maps=AllMaps --NoCompile -stage -pak -archive -archivedirectory="${workspace}/${COOKED_ROOT}"
				"""
				
				//Rename & Copy runtime dependencies
				bat """
					chcp ${WIN_CMD_ENCODING}
					rd ${COOKED_ROOT.replaceAll('/','\\\\')}\\RuyiSDKDemo /S /Q
					ren ${COOKED_ROOT.replaceAll('/','\\\\')}\\WindowsNoEditor RuyiSDKDemo
					xcopy ${RUYI_SDK_CPP}\\lib\\zmq\\libzmq.dll ${COOKED_ROOT.replaceAll('/','\\\\')}\\RuyiSDKDemo /i /y
				"""
			}
			
			post {
				success {
					stage_success env.STAGE_NAME
				}
				failure {
					stage_failed env.STAGE_NAME
				}
			}
		}
			
		stage('Pack'){
			steps{
				bat """
					${RUYI_DEV_ROOT}\\RuyiDev.exe AppRunner --pack --appPath="${COOKED_ROOT}"
				"""
			}
			
			post {
				success {
					stage_success env.STAGE_NAME
				}
				failure {
					stage_failed env.STAGE_NAME
				}
			}
		}
		

		stage('Archive'){
			steps{
				script{
					bat """
						md ${ARCHIVE_ROOT}
						pushd ${DEMO_PROJECT_ROOT}
						git rev-parse HEAD > ${workspace.replaceAll('/','\\\\')}\\${COMMIT_ID_FILE}
						popd
						xcopy ${DEMO_PROJECT_ROOT}\\Pack.zip ${ARCHIVE_ROOT} /i /y
						exit 0
					"""

					echo 'Start archiving artifacts ...'
					archiveArtifacts artifacts: "${ARCHIVE_ROOT}/**/**", onlyIfSuccessful: true
				}
			}
			
			post {
				success {
					stage_success env.STAGE_NAME
				}
				failure {
					stage_failed env.STAGE_NAME
				}
			}
		}
				
		stage('Finalize'){
			steps{
				dir(ARCHIVE_ROOT){
					deleteDir()
				}

				dir(TEMP_DIR){
					deleteDir()
				}
			}
			
			post {
				success {
					stage_success env.STAGE_NAME
				}
				failure {
					stage_failed env.STAGE_NAME
				}
			}
		}
	}
	
	post{
		failure{
			script{
				if(env.FAILURE_STAGE==null)
					env.FAILURE_STAGE = 'Unknown'

				if(params.MAIL_ON_FAILED){
					emailext mimeType:"text/html",
						 replyTo:"Jenkins@playruyi.com",
						 subject:"\u2639 ${env.JOB_NAME} #${env.BUILD_NUMBER} failed in [${env.FAILURE_STAGE}] stage.",
						 to:"${MAIL_RECIPIENT}",
						 body:'${JELLY_SCRIPT,template="jenkins-email-ext-clangScanReport-template.jelly"}'
				}
			}
		}
	}
}

void stage_success(stage){
	echo "[${stage}] stage successfully completed"
}

void stage_failed(stage){
	echo "[${stage}] stage failed"
	
	if(env.FAILURE_STAGE==null)
		env.FAILURE_STAGE = stage
}