//#ifdef _x_PLUGININFO_H_x_
//#define _x_PLUGININFO_H_x_

// PluginInfo
typedef struct _PLUGININFO {
	int type;						// PluginType
	char name[100];					// PluginName
	char version[100];				// PluginVersion
	char versionDate[100];			// PluginVersion Date
	char authorName[100];			// author's Name
	char authorEmail[100];			// author's E-mail
	char authorUrl[100];			// author's URL
	char comment[1024*10];			// Comment
} PLUGININFO;

// PluginType
typedef enum _PLUGIN_TYPE
{
	PLT_TOOLBAR=1,					// Toolbar
	PLT_EXPLORERBAR,				// Explorer
	PLT_STATUSBAR,					// Statusbar
	PLT_OPERATION,					// Operation
}PLUGIN_TYPE;

#define PLGUIN_TYPECNT	4			// PlguinTypeCount

//#endif