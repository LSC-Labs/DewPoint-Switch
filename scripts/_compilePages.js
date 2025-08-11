import fs from 'fs';
import path from 'path';
import gulp from 'gulp';
import gzip from 'gulp-gzip';
import debug from 'gulp-debug';
import htmlmin from 'gulp-htmlmin';
import uglify from 'gulp-uglify';
import cleancss from 'gulp-clean-css';
import jsonmin from 'gulp-jsonmin';
import flatmap from 'gulp-flatmap';
import pump from 'pump';
import { CConfig } from './_common.js';

const DEFAULTS = {
    "locations": {
        "webSource": "src/web",
        "webDist"  : "dist/web",
        "webPacked": "dist/tmp",
        "includes" : "dist/include"
    }
}
var Settings = new CConfig(DEFAULTS);


// #region Minimize functions

function minimizeHTML(cb) {
    let strSourcePath = Settings.getWebSourcePath() + "/*.html";
    let strTargetPath = Settings.getWebDistPath();
    let strPackedPath = Settings.getWebPackedPath();

    gulp.src(strSourcePath)
    .pipe(debug({ title: 'min HTML :'}))
    .pipe(htmlmin({collapseWhitespace: true, minifyJS: true, removeComments:true}))
    .pipe(gulp.dest(strTargetPath))
    .pipe(gzip({append:true}))
    .pipe(gulp.dest(strPackedPath));
    cb();
}

function minimizeScripts(cb) {
    let strSourcePath = Settings.getWebSourcePath() + "/js/*.js";
    let strTargetPath = Settings.getWebDistPath() + "/js";
    let strPackedPath = Settings.getWebPackedPath();
    
    gulp.src(strSourcePath)
    .pipe(debug({ title: 'min JS   :'}))
    .pipe(uglify())
    .pipe(gulp.dest(strTargetPath))
    .pipe(gzip({append:true}))
    .pipe(gulp.dest(strPackedPath));
    cb();
}

function minimizeCSS(cb) {
    let strSourcePath = Settings.getWebSourcePath() + "/css/*.css";
    let strTargetPath = Settings.getWebDistPath() + "/css";
    let strPackedPath = Settings.getWebPackedPath();
    console.log("CSS out : " + strTargetPath);
    gulp.src(strSourcePath)
    .pipe(debug({ title: 'min CSS  :'}))
    .pipe(cleancss())
    .pipe(gulp.dest(strTargetPath))
    .pipe(gzip({append:true}))
    .pipe(gulp.dest(strPackedPath));
    cb();
}

function minimizeLanguages(cb) {
    let strSourcePath = Settings.getWebSourcePath() + "/i18n/*.json";
    let strTargetPath = Settings.getWebDistPath() + "/i18n";
    let strPackedPath = Settings.getWebPackedPath();
    gulp.src(strSourcePath)
    .pipe(debug({ title: 'min i18n :'}))
    .pipe(jsonmin())
    .pipe(gulp.dest(strTargetPath))
    .pipe(gzip({append:true}))
    .pipe(gulp.dest(strPackedPath));
    cb();
}

// #endregion

// #region build of cpp header information of packed files
/**
 * buildHeaderFiles()
 * - build the c includeable header files of the zipped files (*.gz)
 * Only this files are needed by the program - if you choose to include them into the code.
 * @param {*} cb 
 * @returns 
 */
function buildHeaderFiles(cb) {
    let strScanMask = Settings.getWebPackedPath() + "*/*.gz";
    console.log("Building headers files from : " + strScanMask)
    return(
        pump([
            gulp.src(strScanMask),
            debug({ title: 'building :'}),
            flatmap( function(oStream, oFile) {
                let strFileName = path.basename(oFile.path);
                let strTargetPath = path.join(Settings.getIncludePath(),"web") ;
                if(!fs.existsSync(strTargetPath))  {
                    fs.mkdirSync(strTargetPath,{ recursive: true });
                }
                let strTargetFile = path.join(strTargetPath,strFileName + ".h");
                console.log(" --- writing : " + strTargetFile);
                let oWS = fs.createWriteStream(strTargetFile);
                oWS.on("error", function(err) {
                    gutil.log(err);
                });
                console.log("writing: " + strTargetFile)
                let oData = oFile.contents;
                oWS.write('#pragma once\n');
                oWS.write("// -------------------------------------------------------------------\n")
                oWS.write('// LSC-Labs auto generated file : ' + strFileName + '.h\n');
                oWS.write('// ' + Date() + '\n');
                oWS.write("// -------------------------------------------------------------------\n")
                oWS.write('// do not touch - changes will be overwritten...' + '\n');
                oWS.write("// -------------------------------------------------------------------\n")
                oWS.write("#define " + strFileName.replace(/\.|-/g, "_") + "_len " + oData.length + "\n");
                oWS.write("const uint8_t " + strFileName.replace(/\.|-/g, "_") + "[] PROGMEM = {")
                
                for (let i = 0; i < oData.length; i++) {
                    if (i % 1000 == 0) oWS.write("\n");
                    oWS.write('0x' + ('00' + oData[i].toString(16)).slice(-2));
                    if (i < oData.length - 1) oWS.write(',');
                }
    
                oWS.write("\n};")
                oWS.end();
    
                return oStream;
            })
        ])
    )
}

// #endregion


export async function runCompilePages(cb, oSettings) {
    console.log("---- compile....");
    Settings.addConfig(oSettings);
    
    const runJob = gulp.series(   
                                    minimizeHTML,
                                    minimizeCSS,
                                    minimizeScripts,
                                    minimizeLanguages,
                                    buildHeaderFiles
                                );
    return await runJob();
}