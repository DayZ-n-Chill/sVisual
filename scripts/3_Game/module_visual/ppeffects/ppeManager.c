typedef set<ref PPEParams> TPPEParamsList;
typedef set<ref PPEAnimatedParams> TPPEAnimatedParamsList;

class PPEManager {
	
	protected static float m_time;
	static bool m_debugMode = false;
	
	//=========== Depth of Field ==============
	protected static ref DDOFPreset m_DDOF = new DDOFPreset(); //default Dynamic Depth of Field values
	protected static bool m_DDOF_Enabled = true; // Dynamic Depth of Field (used in 1st person view)	
	protected static float m_DDOFVelocity[1];
	
	protected static bool m_peakMitigationEnabled = false; // Dynamic Depth of Field (used in 3rd person view as shoulder peak mitigation)

	protected static ref DoFPreset m_weaponDOF = new WeaponDOFPreset(); //default Depth of Field while aiming down sight

	//=========== Motion Blur ==============
	protected static bool m_MotionBlur_Enabled = true;
	
	//=========== Bloom ==============
	protected static bool m_Bloom_Enabled = false;
	
	//=========== Materials ============== (unused)
	protected static Material glow;
	protected static Material radialBlur;
	protected static Material motionBlur;
	protected static Material gaussBlur;
	protected static Material chromAber;	
	protected static Material filmgrain;
	
	//=========== PostProcess Effects Parameters ==============
	protected static ref PPEParams m_defaultPPE = new PPEParams(); //default params with no modifiers
	protected static ref PPEParams m_vanillaPPE = new PPEParams(); // used by vanilla PPEffects class
	
	protected static ref TPPEParamsList m_persistentPPE = new TPPEParamsList; //all "non-animated" params
	protected static ref TPPEAnimatedParamsList m_animatedPPE = new TPPEAnimatedParamsList; //all animated params
	
	protected static ref PPEParams m_resultPPE = new PPEParams(); // final ppe params after the merge
	
	//=========================================================
	
	
	////////////////////////////////////////////////////////////
	//				INIT
	////////////////////////////////////////////////////////////
	
	void PPEManager(){
		
	}
	
	void ~PPEManager(){
		onDestroy();
	}
	
	static void onInit(){
		loadMaterials();	
		loadDefaultParams();	
		activateInitialPPE();
	}
		
	protected static void onDestroy(){
		
	}
	
	//@todo find a way of caching material instead of getting it every time
	protected static void loadMaterials(){
		glow = GetGame().GetWorld().GetMaterial(MaterialNames.GLOW);
		radialBlur = GetGame().GetWorld().GetMaterial(MaterialNames.RADIAL_BLUR);
		motionBlur = GetGame().GetWorld().GetMaterial(MaterialNames.MOTION_BLUR);
		gaussBlur = GetGame().GetWorld().GetMaterial(MaterialNames.GAUSS_BLUR);
		chromAber = GetGame().GetWorld().GetMaterial(MaterialNames.CHROM_ABER);
		filmgrain = GetGame().GetWorld().GetMaterial(MaterialNames.FILM_GRAIN);
	}
	
	protected static void loadDefaultParams(){
		m_defaultPPE.init(new PPEDefaultPreset);
		//m_resultPPE.init(new PPEDefaultPreset);
	}
	
	protected static void activateInitialPPE(){
		//activate(m_vanillaPPE); //merged manually
	}
	
	
	
	
	
	
	

	////////////////////////////////////////////////////////////
	//				ACTIVATION / DEACTIVATION
	////////////////////////////////////////////////////////////
	
	//@todo check for IsClient inside activate/deactivate ?
	
	/**
	* @brief Add a post process effect modifier 
	* 	@params params \p PPEParams - Modifier Parameters to be added
	*/
	static void activate(PPEParams params){
		SLog.d("ACTIVATING " + params,"PPEManager::activate", 0, m_debugMode);
		if(params.IsInherited(PPEAnimatedParams)){
			PPEAnimatedParams ppeAp = PPEAnimatedParams.Cast(params);
			m_animatedPPE.Insert(ppeAp); // https://www.youtube.com/watch?v=Ct6BUPvE2sM
			ppeAp.start();
		}else{
			m_persistentPPE.Insert(params);
		}
		params.onActivate();
	}
	
	/**
	* @brief Remove a post process effect modifier 
	* 	@params params \p PPEParams - Modifier Parameters to be removed
	*/
	static void deactivate(PPEParams params){
		SLog.d("DEACTIVATING " + params,"PPEManager::deactivate", 0, m_debugMode);
		if(params.IsInherited(PPEAnimatedParams)){
			PPEAnimatedParams ppeAp = PPEAnimatedParams.Cast(params);
			ppeAp.stop();
			m_animatedPPE.Remove(m_animatedPPE.Find(ppeAp));
		}else{
			m_persistentPPE.Remove(m_persistentPPE.Find(params));			
		}
		params.onDeactivate();
	}
	
	/**
	* @brief Remove all modifiers
	*/
	static void deactivateAll(){
		deactivateAllPersitentPPE();
		deactivateAllAnimations();		
	}
	
	static void deactivateAllPersitentPPE(){
		m_persistentPPE.Clear();
	}
	
	static void deactivateAllAnimations(){
		foreach(auto ppeAp : m_animatedPPE){
			ppeAp.stop();
		}
		m_animatedPPE.Clear();
	}
	
	/**
	*	@brief Immediately apply default params
	*/
	static void applyDefault(){
		applyParams(m_defaultPPE);
	}
	
	
	
	
	
	
	
	
	
	
	
	////////////////////////////////////////////////////////////
	//				UPDATING PROCESS
	////////////////////////////////////////////////////////////
	
	static void onUpdate(float delta_time){
		m_time += delta_time;
		
		animateParams(delta_time);
		mergeResult();		
		if(m_resultPPE.hasChanged()){
			applyParams(m_resultPPE);
		}
	}	
		
	/**
	* @brief Iterate over all animated parameters and animate
	*/
	protected static void animateParams(float deltaTime){
		TPPEAnimatedParamsList toDeactivate = new TPPEAnimatedParamsList;
		foreach(PPEAnimatedParams ap : m_animatedPPE){
			if(!ap.hasStopped()){
				if(!ap.isPaused()){
					ap.animate(deltaTime);
				}
			}else{
				if(PPETimedParams.Cast(ap) && PPETimedParams.Cast(ap).shouldDeactivateOnStop()){
					toDeactivate.Insert(ap);
				}
			}
		}
		
		foreach(PPEAnimatedParams apToDeactivate : toDeactivate){
			SLog.d("" + apToDeactivate + " has stopped. Deactivating it...","PPEManager::animateParams",0,m_debugMode);
			deactivate(apToDeactivate);
		}
	}
	
	protected static void mergeResult(){

		//if the final result of PPEffects are altered, return to default values at RESET_SPEED
		if(!m_resultPPE.equals(m_defaultPPE)){
			m_resultPPE.merge(m_defaultPPE, PPEMergeFlags.SIMPLE, PPEConstants.RESET_SPEED);
		}
		
		//Apply persistent PPEffects
		foreach(PPEParams p : m_persistentPPE){
			//if(p.hasChanged()){
				m_resultPPE.merge(p, PPEMergeFlags.MAX | PPEMergeFlags.INTERPOLATE);
				p.onMerge();
			//}
		}
		
		//Apply animated PPEffects
		foreach(PPEAnimatedParams ap : m_animatedPPE){
			if(ap.hasChanged()){
				m_resultPPE.merge(ap, PPEMergeFlags.MAX);
				ap.onMerge();
			}
		}
		
		//Apply vanilla effects
		//if(m_vanillaPPE.hasChanged()){
			m_resultPPE.merge(m_vanillaPPE, PPEMergeFlags.MAX, PPEConstants.VANILLA_COEFF);
		//}
		
	}
	
	/**
	* @brief Iterate and apply the parameters
	* 	@params params \p PPEParams - Parameters to be applied
	*/
	protected static void applyParams(PPEParams params){
		if(m_debugMode){
			SLog.d("params have changed, updating...","",0);
			SLog.d("==========================","",0);
			SLog.d("staticPPE     has " + m_persistentPPE.Count() + " total active PPEParams","",1);
			SLog.d("m_animatedPPE has " + m_animatedPPE.Count() + " total active PPEParams","",1);				
			params.debugPrint();
		}
		applyFloatParams(params);
		applyColorParams(params);
		params.onApply();
	}

	
	/**
	* @brief Iterate over all (float) parameters for each material and apply them	
	*/
	protected static void applyFloatParams(PPEParams params){
		TPPEFloatParamsMap ppeParams = params.getFloatParams();
		foreach(auto ppeMaterial, auto ppeParam : ppeParams){
			foreach(auto ppeParamName, auto ppeParamValue : ppeParam){
				applyPPEParamTo(ppeParamName, ppeParamValue, ppeMaterial);
			}
		}

	}
	
	/**
	* @brief Iterate over all (TPPEColor) parameters for each material and apply them
	*/
	protected static void applyColorParams(PPEParams params){
		TPPEColorParamsMap ppeParams = params.getColorParams();
		foreach(auto ppeMaterial, auto ppeParam : ppeParams){
			foreach(auto ppeParamName, auto ppeParamValue : ppeParam){
				applyPPEParamTo(ppeParamName, ppeParamValue, ppeMaterial);
			}
		}	
	}
	
	/**
	* @brief Apply a float parameter to a material
	* 	@param paramName \p TPPEParamName - Name of the parameter
	* 	@param paramValue \p float - Value of the parameter
	* 	@param mat \p TPPEMaterial - Material to apply parameter to
	* 	@return \p bool - return true if parameter has been applied succesfully, false otherwise
	*/
	protected static bool applyPPEParamTo(TPPEParamName paramName, float paramValue, TPPEMaterial mat){
		SLog.d("Mat: " + mat + "\t\t : " + paramName + " : " + paramValue, "",1, m_debugMode);
		return GetGame().GetWorld().GetMaterial(mat).SetParam(paramName,paramValue);
	}
	
	
	/**
	* @brief Apply a TPPEColor parameter to a material
	* 	@param paramName \p TPPEParamName - Name of the parameter
	* 	@param paramValue \p TPPEColor - Color to be applied, which will be converted in a static array and then applied
	* 	@param mat \p TPPEMaterial - Material to apply parameter to
	* 	@return \p bool - return true if parameter has been applied succesfully, false otherwise
	*/
	protected static bool applyPPEParamTo(TPPEParamName paramName, TPPEColor paramValue, TPPEMaterial mat){
		SLog.d("Mat: " + mat + "\t\t : " + paramName + " : " + paramValue, "",1, m_debugMode);
		float color[4];
		color[0] = paramValue[0];
		color[1] = paramValue[1];
		color[2] = paramValue[2];
		color[3] = paramValue[3];
		return GetGame().GetWorld().GetMaterial(mat).SetParam(paramName,color);
	}	
	
	
	/**
	* @brief Get the (float) default value of a parameter
	* 	@param ppeMaterial \p TPPEMaterial - Material of parameter
	* 	@param ppeParamName \p TPPEParamName - Name of parameter
	* 	@return \p float - default parameter value
	*/
	static float getFloatDefaultValue(TPPEMaterial ppeMaterial, TPPEParamName ppeParamName){
		return m_defaultPPE.getFloatParam(ppeMaterial, ppeParamName);
	}
	
	/**
	* @brief Get the (TPPEColor) default value of a parameter
	* 	@param ppeMaterial \p TPPEMaterial - Material of parameter
	* 	@param ppeParamName \p TPPEParamName - Name of parameter
	* 	@return \p TPPEColor - default parameter value
	*/
	static TPPEColor getColorDefaultValue(TPPEMaterial ppeMaterial, TPPEParamName ppeParamName){
		return m_defaultPPE.getColorParam(ppeMaterial, ppeParamName);
	}
	
	

	
	////////////////////////////////////////////////////////////
	//				UTILS
	////////////////////////////////////////////////////////////
	
	/**
	* @brief Convert four float values to a TPPEColor
	* 	@param R \p float - Red
	* 	@param G \p float - Green
	* 	@param B \p float - Blue
	* 	@param A \p float - Alpha
	* 	@return \p TPPEColor - return converted TPPEColor
	*/
	static TPPEColor getPPEColor(float R, float G, float B, float A){
		TPPEColor color = new TPPEColor;
		color.InsertAt(R, 0); //Red
		color.InsertAt(G, 1); //Green
		color.InsertAt(B, 2); //Blue
		color.InsertAt(A, 3); //Alpha
		return color;
	}
	
	/**
	* @brief Mix the values of two TPPEColor
	* 	@param color1 \p TPPEColor - First color
	* 	@param color2 \p TPPEColor - Second color
	* 	@return \p TPPEColor - return mixed TPPEColor
	*/
	static TPPEColor mixColors(TPPEColor color1, TPPEColor color2, float coeff = 0.5){
		if(!color1 || !color2) return null;
		
		TPPEColor result = new TPPEColor;
		result.InsertAt(Math.Lerp(color1[0], color2[0], coeff), 0); //Red
		result.InsertAt(Math.Lerp(color1[1], color2[1], coeff), 1); //Green
		result.InsertAt(Math.Lerp(color1[2], color2[2], coeff), 2); //Blue
		result.InsertAt(Math.Lerp(color1[3], color2[3], coeff), 3); //Alpha
		if(m_debugMode){
			SLog.d("","mixColors()",0);
			SLog.d(string.Format("Color1 : %1 %2 %3 %4", color1[0],color1[1],color1[2],color1[3]) ,"",1);
			SLog.d(string.Format("Color2 : %1 %2 %3 %4", color2[0],color2[1],color2[2],color2[3]) ,"",1);
			SLog.d(string.Format("Result : %1 %2 %3 %4", result[0],result[1],result[2],result[3]) ,"",1);
		}
		return result;
	}
	
	
	
	////////////////////////////////////////////////////////////
	//				DEBUG
	////////////////////////////////////////////////////////////
	static TPPEAnimatedParamsList getAnimations(){
		return m_animatedPPE;
	}
	
	static void debugPrintAll(bool printParamsValues = false){
		SLog.d("debugPrintAll","PluginPPEffect",0);
		
		SLog.d("======================================================", "",0);
		SLog.d("-------------------- m_defaultPPE --------------------", "",1);
		m_defaultPPE.debugPrint();
				
		SLog.d("-------------------- m_persistentPPE --------------------", "",1);
		foreach(PPEParams p : m_persistentPPE){
			SLog.d(p);
			if (printParamsValues) p.debugPrint();
		}
		
		SLog.d("-------------------- m_animatedPPE --------------------", "",1);
		foreach(PPEParams ap : m_animatedPPE){
			SLog.d(ap);
			if (printParamsValues) ap.debugPrint();
		}
		
		SLog.d("-------------------- m_vanillaPPE --------------------", "",1);
		if (true) m_vanillaPPE.debugPrint();
		
		SLog.d("-------------------- m_resultPPE --------------------", "",1);
		m_resultPPE.debugPrint();
		SLog.d("======================================================", "",0);

	}
	
	
	
	
	////////////////////////////////////////////////////////////
	//				VANILLA
	////////////////////////////////////////////////////////////
	
	/**
	*	@deprecated
	*	Methods used by vanilla code... Planned to be removed when vanilla code is replaced
	*/

	static void vanillaSetGausBlur(float value){
		m_vanillaPPE.setGausBlur(value);
	}
	
	static void vanillaSetLensChromAber(float value){
		m_vanillaPPE.setLensChromAber(value);
	}
	
	static void vanillaSetOverlay(float factor, TPPEColor color){
		m_vanillaPPE.setOverlay(factor, color);
	}
	
	static void vanillaSetLens(float intensity, float centerX, float centerY, float chromAberIntensity){
		m_vanillaPPE.setLens(intensity, centerX, centerY, chromAberIntensity);
	}
	
	static void vanillaSetVignette(float intensity, TPPEColor color){
		m_vanillaPPE.setVignette(intensity, color);
	}
	
	static void vanillaSetSaturation(float saturation){
		m_vanillaPPE.setSaturation(saturation);
	}
	
	static void vanillaSetColorization(TPPEColor color){
		m_vanillaPPE.setColorization(color);
	}
	
	static void vanillaSetFilmGrain(float sharpness, float grainSize){
		m_vanillaPPE.setFilmGrain(sharpness, grainSize);
	}
	
	static void vanillaSetBloom(float steepness, float intensity, float threshold){
		m_vanillaPPE.setBloom(steepness, intensity, threshold);
	}
	
	static void vanillaOverrideDOF(bool enable, float focusDistance, float focusLength, float focusLengthNear, float blur, float focusDepthOffset) {
		//applyDOF(enable, focusDistance, focusLength, focusLengthNear, blur, focusDepthOffset);
	}
	
	static void vanillaResetDOFOverride(){
		//resetDOF();
	}
	
	static void vanillaAddPPMask(float ndcX, float ndcY, float ndcRadius, float ndcBlur) {
		requestOpticMask(ndcX, ndcY, ndcRadius, ndcBlur);
	}
	
	static void vanillaResetPPMask() {
		resetMask();
	}
	
	
	////////////////////////////////////////////////////////////
	//				DEPTH of FIELD
	////////////////////////////////////////////////////////////
	
	static float getDDOFStrength(){
		return m_DDOF.blurStrength;
	}
	
	static float getDDOFMaxDistance(){
		return m_DDOF.focusMaxDistance;
	}
	
	static float getDDOFMinDistance(){
		return m_DDOF.focusMinDistance;
	}
		
	static bool isDDOFEnabled(){
		return m_DDOF_Enabled;
	}
	
	static bool isPeakMitigationEnabled(){
		return m_peakMitigationEnabled;
	}
	
	static void setDDOFBlurStrength(float blurStrength){
		if(blurStrength <= 0){
			m_DDOF.blurStrength = 0;
			disableDDOF();
		}else{
			m_DDOF.blurStrength = blurStrength;
			enableDDOF();
		}
	}
		
	static void requestDDOF(float focusDistance, float pDt){
		m_DDOF.focusDistance = Math.SmoothCD(m_DDOF.focusDistance, focusDistance, m_DDOFVelocity, 0.15, 1000, pDt); //smooth the focus distance over time
		applyDOF(m_DDOF);
	}
	
	static void enableDDOF(){
		m_DDOF_Enabled = true;
	}
	
	static void disableDDOF(){
		m_DDOF_Enabled = false;
		resetDOF();
	}
	
	static void enablePeakMitigation(){
		m_peakMitigationEnabled = true;
	}
	
	static void disablePeakMitigation(){
		m_peakMitigationEnabled = false;
		resetDOF();
	}
	
	/**
	*	@brief Reset Depth of Field
	*/
	static void resetDOF(){
		applyDOF(false, Math.Clamp(m_DDOF.focusDistance, m_DDOF.focusMinDistance, m_DDOF.focusMaxDistance), m_DDOF.focusLength, m_DDOF.focusLengthNear, m_DDOF.blurStrength, m_DDOF.focusDepthOffset);
	}
	
	protected static void applyDOF(DoFPreset dof){
	
		applyDOF(true, Math.Clamp(dof.focusDistance, dof.focusMinDistance, dof.focusMaxDistance), dof.focusLength, dof.focusLengthNear, dof.blurStrength, dof.focusDepthOffset);
	}
	
	protected static void applyDOF(bool enabled, float focusDistance, float focusLength, float focusLengthNear, float blurStrength, float focusDepthOffset){
		//SLog.d(string.Format("Distance: %1 | Blur : %2",focusDistance,blurStrength));
		GetGame().OverrideDOF(enabled, focusDistance, focusLength, focusLengthNear, blurStrength, focusDepthOffset);
	}
	
	
	
	
	
	
	////////////////////////////////////////////////////////////
	//				MOTION BLUR
	////////////////////////////////////////////////////////////
	
	static bool isMotionBlurEnabled(){
		return m_MotionBlur_Enabled;
	}
	
	static float getMotionBlurStrength(){
		return m_defaultPPE.getFloatParam(MaterialNames.MOTION_BLUR, PPEParamNames.MOTION_BLUR_POWER);
	}
	
	static void setMotionBlurStrength(float strength){
		if(strength <= 0){
			m_MotionBlur_Enabled = false;
			m_defaultPPE.setMotionBlurPower(0.0);
		}else{
			m_MotionBlur_Enabled = true;
			m_defaultPPE.setMotionBlurPower(strength);
		}
		
	}
	
	////////////////////////////////////////////////////////////
	//				BLOOM
	////////////////////////////////////////////////////////////
	
	static bool isBloomEnabled(){
		return m_Bloom_Enabled;
	}
	
	static float getBloomStrength(){
		return m_defaultPPE.getFloatParam(MaterialNames.GLOW, PPEParamNames.BLOOM_STEEPNESS);
	}
	
	static void setBloomStrength(float strength){
		if(strength <= 0){
			m_Bloom_Enabled = false;
			m_defaultPPE.setBloomSteepness(0.0);
		}else{
			m_Bloom_Enabled = true;
			m_defaultPPE.setBloomSteepness(strength);
		}
		
	}
	
	////////////////////////////////////////////////////////////
	//				OPTICS (LENSES AND MASKS)
	////////////////////////////////////////////////////////////

	static void requestOpticMask(float positionX, float positionY, float radius, float blur){
		GetGame().AddPPMask(positionX, positionY, radius, blur);
	}
		
	static void resetMask(){
		GetGame().ResetPPMask();
	}

}