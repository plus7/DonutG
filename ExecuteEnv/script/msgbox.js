var donut = Components.classes["@tnose.net/donut/api-service;1"]
    .getService(Components.interfaces.donutIAPI);

donut.messageBox("text", "caption", 0);

donut.messageBox(donut.exePath.path, "donut.exePath", 0);