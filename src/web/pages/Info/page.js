/**
  @PageBuilder:register(InfoPage,CInfoPage)
 */

class CInfoPage extends CPageHandler {
    /**
     * 
     * @param {CAPP} oApp 
     */
    constructor(oApp) {
        // don't use this, before initialized !
        super("info",new CElement("#InfoPage"),oApp.Config);
        this._App = oApp;
       // this.Translator = new CTranslator();
    }

    _createThermo(oBaseElement, oData) {
        let oParent = CElement.asInstance(oBaseElement);
        oParent.html(""); 
        let oThermo = oParent.cce("div").ac("thermo");
        let oGlas   = oThermo.cce("div").ac("thermo-glas");
        let oLiquid = oGlas.cce("div").ac("thermo-liquid");
        let oLabel  = oThermo.cce("div").ac("thermo-label");
        if(oData.label) oLabel.setText(oData.label)
        oLiquid.getBase().style.height = oData.height;
        switch(oData.type) {
            case "T" :  oLiquid.ac("thermo-temp");
                        break;
            case "H" : oLiquid.ac("thermo-humidity");
                        break;
        }
        oGlas.ac("thermo-glas-active");
        return(oThermo);
    }

    calcDisplayPercent(fData, fMin = -20, fRange = 60) {
        return(((fData - fMin) * 100 / fRange).toFixed() + "%")
    }

    /**
     * 
     * @param {*} pView 
     * @param {*} pApp 
     * @returns 
     * @overload
     */
    loadPageConfig(pView, pApp) {
        let oStatus = pApp.DeviceStatus.dewswitch;
        let oID = pApp.DeviceStatus.sensorID ?? {};
        let oOD = pApp.DeviceStatus.sensorOD ?? {};
        
        let nMaxTemp = 80;
        let nMinTemp = -20; 
        let nTempRange = 60;
        let nDefaultTemp = nMinTemp;
        let nDefaultHum = 0;
        this._createThermo(pView.gel("#intTemp"), {
            type: "T",
            label: (oID.temp_c ? oID.temp_c.toFixed(1) : "--")  + "°",
            height: this.calcDisplayPercent((oID.temp_c ?? nDefaultTemp),nMinTemp,nTempRange)
        })
        this._createThermo(pView.gel("#intHumidity"), {
            type: "H",
            label: (oID.hum ?? nDefaultHum).toFixed(1) + "%",
            height: this.calcDisplayPercent((oID.hum ?? nDefaultHum),0,100)
        })
        this._createThermo(pView.gel("#extTemp"), {
            type: "T",
            label: (oOD.temp_c ? oOD.temp_c.toFixed(1) : "--.-")  + "°",
            height: this.calcDisplayPercent((oOD.temp_c ?? nDefaultTemp),nMinTemp,nTempRange)
        })
        this._createThermo(pView.gel("#extHumidity"), {
            type: "H",
           label: (oOD.hum ?? nDefaultHum).toFixed(1) + "%",
            height: this.calcDisplayPercent((oOD.hum ?? nDefaultHum),0,100)
        })
        pView.gel("#dewPointID").html((oID.dew_point ?? 0).toFixed(1) + "°C" );
        pView.gel("#tempID").html((oID.temp_c ?? 0).toFixed(1) + "°C" );
        pView.gel("#humID").html((oID.hum ?? 0).toFixed() + "%" );

        pView.gel("#dewPointOD").html((oOD.dew_point ?? 0).toFixed(1) + "°C" );
        pView.gel("#tempOD").html((oOD.temp_c ?? 0).toFixed(1) + "°C" );
        pView.gel("#humOD").html((oOD.hum ?? 0).toFixed() + "%" );

        return(oStatus);
    }

}