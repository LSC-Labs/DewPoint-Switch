/**
  @PageBuilder:register(StatusPage,CStatusPage)
 */

class CStatusPage extends CPageHandler {
    /**
     * 
     * @param {CAPP} oApp 
     */
    constructor(oApp) {
        // don't use this, before initialized !
        super("status",new CElement("#StatusPage"),oApp.Config);
        this._App = oApp;
       // this.Translator = new CTranslator();
    }

    /**
     * 
     * @param {*} pView 
     * @param {*} pApp 
     * @returns 
     * @overload
     */
    loadPageConfig(pView, pApp) {
        let oStatus = pApp.DeviceStatus;
       
        // set the root element infos - no prefix...
        this.setConfigValues(pView,oStatus);
        for(let strName in oStatus) {
            let oData = oStatus[strName];
            if(oData && Utils.isObj(oData)) {
                this.setConfigValues(pView,oData,strName);
            }
        }

        let oVars = new CVarTable();
        oVars.setVars(pApp.tVars);
        oVars.setVars(oStatus);
        
        // now the usage bar
        let nAvailableSize = oStatus.flash_size - oStatus.fs_total;
        let nPercentUsed = (oStatus.sketch_size * 100) / nAvailableSize;
        let oUsageBar =  pView.sel("#flashUsage");
        if(oUsageBar) {
            oVars.setVar("flash_used_percent",~~nPercentUsed);
            oVars.setVar("flash_available",nAvailableSize);
            oUsageBar.style.width = nPercentUsed + "%";
            oUsageBar.innerText = ~~(oStatus.sketch_size / 1024) + " KB";
            let oUB = EC(oUsageBar);
            let oTranse = new CTranslator();
            let strI18n = "Status.memOK";
            if(nPercentUsed > 50) {
                oUB.rc("usageOK").ac("usageWarn");
                strI18n = "Status.memWarn";
            } else {
                oUB.ac("usageOK").rc("usageWarn");
            }
            oTranse.translateI18n(strI18n,undefined,oVars)
                .then(str => { 
                    oUB.attr("title",str)
                }
            )
        }
        return(oStatus);
    }

}