class SGameConfig {

	static const string CFG_BASENAME = "Cfg_sUDE";
	
	static string getCfgName(){
		return CFG_BASENAME;
	}
	
	/**
	*	@brief Read integer from config.cpp
	*	 @param path \p string - Path to read
	*	 @return int
	*/
	static int getInt(string path){
		int value;
		get(path, value);
		return value;
	}
	
	/**
	*	@brief Read float from config.cpp
	*	 @param path \p string - Path to read
	*	 @return float
	*/
	static float getFloat(string path){
		float value;
		get(path, value);
		return value;
	}
	
	/**
	*	@brief Read boolean from config.cpp
	*	 @param path \p string - Path to read
	*	 @return bool
	*/
	static bool getBool(string path){
		bool value;
		get(path, value);
		return value;
	}
	
	/**
	*	@brief Read string from config.cpp
	*	 @param path \p string - Path to read
	*	 @return string
	*/
	static string getString(string path){
		string value;
		get(path, value);
		return value;
	}
	
	/**
	*	@brief Read vector from config.cpp
	*	 @param path \p string - Path to read
	*	 @return vector
	*/
	static vector getVector(string path){
		vector value;
		get(path, value);
		return value;
	}
	
	/**
	*	@brief Read array of integer from config.cpp
	*	 @param path \p string - Path to read
	*	 @return array<int> - empty if path doesn't exists
	*/
	static TIntArray getIntArray(string path){
		TIntArray value = new TIntArray();
		get(path, value);
		return value;
	}
	
	/**
	*	@brief Read array of float from config.cpp
	*	 @param path \p string - Path to read
	*	 @return array<float> - empty if path doesn't exists
	*/
	static TFloatArray getfloatArray(string path){
		TFloatArray value = new TFloatArray();
		get(path, value);
		return value;
	}
	
	/**
	*	@brief Read array of string from config.cpp
	*	 @param path \p string - Path to read
	*	 @return array<string> - empty if path doesn't exists
	*/
	static TStringArray getStringArray(string path){
		TStringArray value = new TStringArray();
		get(path, value);
		return value;
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
	
	
	private static ref SGameConfigVisual m_visual = new SGameConfigVisual();
	static SGameConfigVisual visual(){
		return m_visual;
	}
	
}
