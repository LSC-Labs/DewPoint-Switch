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
        let oThermo = oParent.cce("div").ac("thermo");
        let oGlas   = oThermo.cce("div").ac("thermo-glas");
        let oLiquid = oGlas.cce("div").ac("thermo-liquid");
        let oLabel  = oThermo.cce("div").ac("thermo-label");
        if(oData.label) oLabel.setText(oData.label)
        oLiquid.getBase().style.height = oData.height;
        switch(oData.type) {
            case "T" :  oLiquid.ac("thermo-temp");
                        break;
            case "H" : oLiquid.ac("thermo-liquid");
                        break;
        }
        return(oThermo);
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
        this._createThermo(pView.gel("#intTemp"), {
            type: "T",
            label: "12°",
            height: "70%"
        })
        this._createThermo(pView.gel("#intHumidity"), {
            type: "H",
            label: "22%",
            height: "22px"
        })
        this._createThermo(pView.gel("#extTemp"), {
            type: "T",
            label: "32°",
            height: "30%"
        })
        this._createThermo(pView.gel("#extHumidity"), {
            type: "H",
            label: "42%",
            height: "42px"
        })
        return(oStatus);
    }

}