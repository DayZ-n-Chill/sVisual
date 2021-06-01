class OptionsMenuSVisual : SOptionsMenuTab{
	
	override string getLayoutName(){
		return "MyMODS/sVisual/scripts/5_Mission/gui/newui/options/sVisual_tab.layout";
	}
	
	protected ref SUserConfigVisual m_sUserConfig;
		
	static const string WN_HEADBOB            = "sude_headbob_setting_";
	static const string WN_HEADBOB_SWITCH_3PP = "sude_headbob_switch_3pp_";
	static const string WN_DOF                = "sude_dof_setting_";
	static const string WN_DOF_SWITCH_3PP     = "sude_dof_switch_3pp_";
	static const string WN_DOF_SWITCH_VEHICLE = "sude_dof_switch_vehicle_";
	static const string WN_MOTIONBLUR         = "sude_motionblur_setting_";
	static const string WN_BLOOM              = "sude_bloom_setting_";
	static const string WN_HEADLEAN           = "sude_roll_setting_";
		
	protected ref SliderWidget				m_headBobSlider;
	protected ref TextWidget				m_headBobValue;
	protected ref CheckBoxWidget            m_headbobSwitch3pp;
	
	protected ref SliderWidget				m_dofSlider;
	protected ref TextWidget				m_dofValue;
	protected ref CheckBoxWidget            m_dofSwitch3PP;
	protected ref CheckBoxWidget            m_dofSwitchVehicle;
	
	protected ref SliderWidget				m_motionBlurSlider;
	protected ref TextWidget				m_motionBlurValue;
	
	protected ref SliderWidget				m_bloomSlider;
	protected ref TextWidget				m_bloomValue;
	
	protected ref SliderWidget				m_headLeanSlider;
	protected ref TextWidget				m_headLeanValue;

	override void onInit(){
		m_sUserConfig = SUserConfig.visual();
		super.onInit();
	}
	
	override void initOptionsWidgets(){
		
		init(WN_HEADBOB, m_headBobSlider, m_headBobValue,
			m_sUserConfig.getHeadbobIntensity(), 
			"#STR_SUDE_LAYOUT_OPTIONS_CAMERA_HEADBOB", 
			"#STR_SUDE_LAYOUT_OPTIONS_CAMERA_HEADBOB_DESCRIPTION");
		
		init(WN_HEADBOB_SWITCH_3PP, m_headbobSwitch3pp,
			m_sUserConfig.isHeadbobEnabledIn3pp());
		
		init(WN_DOF, m_dofSlider, m_dofValue,
			m_sUserConfig.getDDOFIntensity(), 
			"#STR_SUDE_LAYOUT_OPTIONS_CAMERA_DOF", 
			"#STR_SUDE_LAYOUT_OPTIONS_CAMERA_DOF_DESCRIPTION");
		
		init(WN_DOF_SWITCH_3PP, m_dofSwitch3PP,
			m_sUserConfig.isDDOFEnabledIn3PP());
		
		init(WN_DOF_SWITCH_VEHICLE, m_dofSwitchVehicle,
			m_sUserConfig.isDDOFEnabledInVehicle());
			
		init(WN_MOTIONBLUR, m_motionBlurSlider, m_motionBlurValue,
			m_sUserConfig.getMotionBlurIntensity(), 
			"#STR_SUDE_LAYOUT_OPTIONS_CAMERA_MOTIONBLUR", 
			"#STR_SUDE_LAYOUT_OPTIONS_CAMERA_MOTIONBLUR_DESCRIPTION");
		
		init(WN_BLOOM, m_bloomSlider, m_bloomValue,
			m_sUserConfig.getBloomIntensity(), 
			"#STR_SUDE_LAYOUT_OPTIONS_CAMERA_BLOOM", 
			"#STR_SUDE_LAYOUT_OPTIONS_CAMERA_BLOOM_DESCRIPTION");
		
		init(WN_HEADLEAN, m_headLeanSlider, m_headLeanValue,
			m_sUserConfig.getHeadLeanAngle(), 
			"#STR_SUDE_LAYOUT_OPTIONS_CAMERA_ROLL", 
			"#STR_SUDE_LAYOUT_OPTIONS_CAMERA_ROLL_DESCRIPTION");

	}
	
	override void onChange(SliderWidget s, int x, int y, bool finished){
		super.onChange(s, x, y, finished);
		switch(s.GetName()){
			case WN_HEADBOB+"option":{
				UpdateHeadBobOption(s.GetCurrent());
				break;
			}
			case WN_DOF+"option":{
				UpdateDoFOption(s.GetCurrent());
				break;
			}
			case WN_MOTIONBLUR+"option":{
				UpdateMotionBlurOption(s.GetCurrent());
				break;
			}
			case WN_BLOOM+"option":{
				UpdateBloomOption(s.GetCurrent());
				break;
			}
			case WN_HEADLEAN+"option":{
				UpdateRollOption(s.GetCurrent());
				break;
			}
			default: SLog.w("No slider widget name found.","OptionsMenuSVisual::OnChange");
		}
	}
	
	override void onChange(CheckBoxWidget c, int x, int y, bool finished){
		super.onChange(c, x, y, finished);
		switch(c.GetName()){
			case WN_HEADBOB_SWITCH_3PP+"option":
				updateHeadbobSwitch3PP(c.IsChecked());
				break;
			case WN_DOF_SWITCH_3PP+"option":
				updateDOFSwitch3PP(c.IsChecked());
				break;
			case WN_DOF_SWITCH_VEHICLE+"option":
				updateDOFSwitchVehicle(c.IsChecked());
				break;
			default: SLog.w("No checkbox  widget name found.","OptionsMenuSVisual::OnChange");
		}
	}
	
	
	protected void UpdateHeadBobOption( float new_value ){
		m_headBobValue.SetText(new_value.ToString());
		
		m_sUserConfig.setHeadbobIntensity(new_value);
		onConfigChange();
	}
	
	protected void updateHeadbobSwitch3PP(bool checked){		
		m_sUserConfig.setHeadbobEnabledIn3pp(checked);
		onConfigChange();
	}
	
	
	protected void UpdateDoFOption( float new_value ){
		PPEManager.setDDOFBlurStrength(new_value);
		m_dofValue.SetText(new_value.ToString());
		
		m_sUserConfig.setDDOFIntensity(new_value);
		onConfigChange();
	}
	
	protected void updateDOFSwitch3PP(bool checked){		
		m_sUserConfig.setDDOFEnabledIn3PP(checked);
		PPEManager.setDDOFEnabledIn3PP(checked);
		onConfigChange();
	}
	
	protected void updateDOFSwitchVehicle(bool checked){		
		m_sUserConfig.setDDOFEnabledInVehicle(checked);
		PPEManager.setDDOFEnabledInVehicle(checked);
		onConfigChange();
	}
	
	protected void UpdateMotionBlurOption( float new_value ){
		PPEManager.setMotionBlurStrength(new_value);
		m_motionBlurValue.SetText(new_value.ToString());
		
		m_sUserConfig.setMotionBlurIntensity(new_value);
		onConfigChange();
	}
	
	protected void UpdateBloomOption( float new_value ){
		PPEManager.setBloomStrength(new_value);
		m_bloomValue.SetText(new_value.ToString());
		
		m_sUserConfig.setBloomIntensity(new_value);
		onConfigChange();
	}
	
	protected void UpdateRollOption( float new_value){
		m_headLeanValue.SetText(new_value.ToString());
		
		m_sUserConfig.setHeadLeanAngle(new_value);
		onConfigChange();
	}
	
	override void onConfigChange(){ //@todo lol change this shit
		m_sUserConfig.save();
	}
	
}