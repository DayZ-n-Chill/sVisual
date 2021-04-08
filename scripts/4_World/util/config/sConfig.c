class SConfig {

	private static string cfgName = "Cfg_sUDE";
	private static ref SConfigVisual m_visual = new SConfigVisual();
	
	static string getCfgName(){
		return cfgName;
	}
		
	static SConfigVisual visual(){
		return m_visual;
	}
		
	
	static void get(string path, out int value){
		value = GetGame().ConfigGetInt(path);
	}
	
	static void get(string path, out float value){
		value = GetGame().ConfigGetFloat(path);
	}
	
	static void get(string path, out bool value){
		value = GetGame().ConfigGetInt(path);
	}
		
	static void get(string path, out string value){
		GetGame().ConfigGetText(path, value);
	}
	
	static void get(string path, out vector value){
		value = GetGame().ConfigGetVector(path);
	}
	
	static void get(string path, inout TIntArray values){
		GetGame().ConfigGetIntArray(path, values);
	}
	
	static void get(string path, inout TFloatArray values){
		GetGame().ConfigGetFloatArray(path, values);
	}
	
	static void get(string path, inout TStringArray values){
		GetGame().ConfigGetTextArray(path, values);
	}
		
}
