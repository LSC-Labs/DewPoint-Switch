import fs from 'fs';
import path from 'path';
import gulp from 'gulp';
import { getProjectName } from './scripts/_common.js';

// #region Settings, common functions and initialization

var Settings;

console.log(getProjectName());
console.log("=======================================");
if(fs.existsSync("pages.json")) {
    console.log(' - loading config from "pages.json"');
    let strData = fs.readFileSync("pages.json");
    Settings = {...Settings, ...JSON.parse(strData) }
}


// #endregion

// #region tasks...

import {runBuildPages} from './scripts/_buildPages.js';
gulp.task('buildPages', (cb) => {
    console.log("Building pages...");  
    runBuildPages(cb,Settings);
    cb();
});

import {runCompilePages} from './scripts/_compilePages.js';
gulp.task('compilePages', async(cb) => {
    console.log("Compiling pages...");
    runCompilePages(cb,Settings);
});

import {runSyncFiles} from './scripts/_syncFiles.js';
gulp.task('syncFiles', async (cb) => {
    console.log("Syncing files...");
    runSyncFiles(cb,Settings);
    // cb();
});
// #endregion
