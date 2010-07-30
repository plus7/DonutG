//#ifdef _x_PLUGININFO_H_x_
//#define _x_PLUGININFO_H_x_



// PluginInfo
struct PLUGININFO {
	int 	type;						// PluginType
	char 	name[100];					// PluginName
	char 	version[100];				// PluginVersion
	char 	versionDate[100];			// PluginVersion Date
	char 	authorName[100];			// author's Name
	char 	authorEmail[100];			// author's E-mail
	char	 authorUrl[100];			// author's URL
	char 	comment[1024*10];			// Comment
};



// PluginType
enum PLUGIN_TYPE {
	PLT_TOOLBAR=1,					// Toolbar
	PLT_EXPLORERBAR,				// Explorer
	PLT_STATUSBAR,					// Statusbar
	PLT_OPERATION,					// Operation
};


#define PLGUIN_TYPECNT	4			// PlguinTypeCount


//#endif
