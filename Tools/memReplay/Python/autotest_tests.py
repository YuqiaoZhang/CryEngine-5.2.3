import amblelib
import amble_capture

defaultUserParams = [
			("r_ShaderCompilerServer", "10.16.0.21"),
			("r_ShadersRemoteCompiler", 1),
			("r_shaderssubmitrequestline",  1),
			("r_shadersremotecompiler", 1),
			("r_shaderslogcachemisses", 1),
			("r_shadersNoCompile", 1),
			("r_displayinfo", 2),
			("pp_unlockall", 1),
			("g_infiniteammo", 1),
			("log_verbosity", 1),
			("log_WriteToFileVerbosity", 1),
			("sv_servername", "MemStress"),
			("g_data_centre_config", "nottingham"),
			("gl_skip", 1),
			("g_skipIntro", 1),
			("g_EnableDevMenuOptions", 1),
			("g_playerenableddedicatedinput", 1),
			("g_playerusesdedicatedinput", 1),
			("statsagent_debug", 1),
			("sys_no_crash_dialog","1"),
			("sys_WER","0"),
			("sys_dump_type","2"),
			("log_verbosity", 3),
			("log_writetofileverbosity", 3),
			("net_tcp_use_xlsp", 0),
			("g_matchmakingblock", 6502),
			("release_install",0)
		]

mpUserParams = [
			("g_dummyplayersmove", 1),
			("g_dummyplayersfire", 1),
			("g_dummyplayersjump", 1),
			("g_dummyplayerschangeweapon", 1),
			("g_dummyPlayersCommitSuicide", 1),
			("g_dummyplayersshowdebugtext", 0),
			("gl_skipPreSearch", 1),
			("g_gameRules_StartCmd", "spawndummyplayers 11"),
			("g_enableGameSwitchMenu", 0),
			("g_enableInitialLoadoutScreen", 0),
			("g_multiplayerdefault", 1),
			("g_telemetry_gameplay_enabled", 1),
			("g_enableLanguageSelectionMenu", 0),
			("g_enableInitialLoginScreen", 0),
			("net_setonlinemode", "lan"),
			("g_minplayerlimit", 1),
			("net_initLobbyServiceToLan", 1),
			("net_setonlinemode", "lan"),
			("sv_servername", "MemStress"),
			("g_telemetryTags", "MemStress"),
			("g_telemetry_logging", 10),
			]

def loadMap(impl, map, timeout):
	impl.Execute("map " + map)  # Blocks while the command runs
	impl.Execute("log_verbosity 9")
	impl.Execute("log_writetofileverbosity 9")
	amble_capture.countingsleep(impl, timeout)
	impl.Execute("unload")

def loadStressMap(impl, map, timeout):
	rules = "TIA"
	#map = "Multiplayer/%s" % (map)
	impl.Execute("autotest_state_setup state:ATEST_STATE_TEST_FEATURES!outputName:Build_unknown_MemReplay_" + map + "_" + rules + "!file:StressTester!rules:" + rules + "!level:" + map + "!set:setup+infiniteTimeLimit+spawnDummyPlayers11+repeatThese")
	impl.Execute("autotest_enabled 1")
	#impl.Execute("map " + map)  # Blocks while the command runs
	amble_capture.countingsleep(impl, timeout)
	impl.Execute("unload")

def makeSoakTest(level):
	def inner(impl):
		loadStressMap(impl, level, 2 * 60 * 60)
	return { "cvars" : defaultUserParams + mpUserParams, "handler" : inner }

def makeLevelLoadTest(level):
	def inner(impl):
		print "Loading map '%s'..." % (level)
		loadMap(impl, level, 1 * 60)
	return { "cvars" : defaultUserParams, "handler" : inner }

def makeMpRoundCycleTest(level, timeLimit=3600):
	def inner(impl):
		rules = "TIA"
		map = level
		impl.Execute("autotest_state_setup state:ATEST_STATE_TEST_FEATURES!outputName:Build_" + impl.InstalledBuildId + "_MemReplay_" + map + "_" + rules + "!file:StressTester!rules:" + rules + "!level:" + map + "!set:setup+repeat2MinThese11")
		impl.Execute("autotest_enabled 1")
		#impl.Execute("map " + map)  # Blocks while the command runs
		amble_capture.countingsleep(impl, timeLimit)
		impl.Execute("unload")
	return { "cvars" : defaultUserParams + mpUserParams, "handler" : inner }

def makeMpRoundCycleTest10(level, timeLimit):
	def inner(impl):
		rules = "TIA"
		map = level
		impl.Execute("autotest_state_setup state:ATEST_STATE_TEST_FEATURES!outputName:Build_" + impl.InstalledBuildId + "_MemReplay_" + map + "_" + rules + "!file:StressTester!rules:" + rules + "!level:" + map + "!set:setup+repeatThese11")
		impl.Execute("autotest_enabled 1")
		#impl.Execute("map " + map)  # Blocks while the command runs
		amble_capture.countingsleep(impl, timeLimit)
		impl.Execute("unload")
	return { "cvars" : defaultUserParams + mpUserParams, "handler" : inner }

def makeChainLoadTest():
	def inner(impl):
		impl.Execute("exec AutoTestChain.cfg")
		impl.Execute("demo_StartDemoChain Test_ChainLevels.txt")
		impl.Execute("g_testStatus 1")
		impl.Execute("demo_quit 0")
		impl.Execute("demo_num_runs 10")
		impl.Execute("demo_finish_cmd \"g_testStatus 0\"")
		timeout = 10000
		duration = amblelib.timedWaitOnCVAR(impl, "g_testStatus", 0, timeout, ".")
		if duration >= 0 and duration < timeout:
			print("Finished test, shutting down.")
			impl.Execute("disconnect")
	return { "cvars" : defaultUserParams, "handler" : inner }

