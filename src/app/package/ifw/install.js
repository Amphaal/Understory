function Component() {
    installer.installationFinished.connect(this, Component.prototype.installationFinishedPageIsShown);
    installer.finishButtonClicked.connect(this, Component.prototype.installationFinished);
}

Component.prototype.getPathToApp = function() {
    var appName = installer.value("Title");
    var exePath = installer.value("TargetDir") + "/bin/" + appName;

    return exePath;
}

Component.prototype.createOperations = function() {
    component.createOperations();

    //create a shortcut on windows
    if (systemInfo.productType === "windows") {
        var exe_suffix = ".exe";
        component.addOperation("CreateShortcut", Component.prototype.getPathToApp() + exe_suffix, "@DesktopDir@/@Title@.lnk");
        component.addOperation("CreateShortcut", Component.prototype.getPathToApp() + exe_suffix, "@StartMenuDir@/@Title@.lnk");
    }
}

Component.prototype.installationFinishedPageIsShown = function() {
    try {
        if (installer.isInstaller() && installer.status == QInstaller.Success) {

            installer.addWizardPageItem( component, "EndInstallerForm", QInstaller.InstallationFinished );
            
        }
    } catch(e) {
        console.log(e);
    }
}

Component.prototype.installationFinished = function() {
    try {
        if (installer.isInstaller() && installer.status == QInstaller.Success) {

            //LaunchBox
            var isLaunchBoxChecked = component.userInterface("EndInstallerForm").LaunchBox.checked;
            if (isLaunchBoxChecked) {
            QDesktopServices.openUrl("file:///" + Component.prototype.getPathToApp());
            }

        }
    } catch(e) {
        console.log(e);
    }
}