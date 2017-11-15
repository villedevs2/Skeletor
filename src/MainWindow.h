#pragma once

#include "QMainWindow"
#include "QAction"
#include "qfiledialog.h"
#include "qboxlayout.h"
#include "qmenubar.h"

#include "GLWidget.h"
#include "assert.h"
#include "FBXMesh.h"
#include "VMFExport.h"
#include "AnimTimeline.h"
#include "AnimList.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

	QMenu *m_mainWindowMenu;

public:
	MainWindow();
	~MainWindow();

	void reset();

protected:
	void closeEvent(QCloseEvent *event);
	void keyReleaseEvent(QKeyEvent *event);

private:
	void createActions();
	void createMenus();
	void createToolbars();
	void createStatusbar();
	
	QMenu *m_fileMenu;
	QAction *m_importAction;
	QAction *m_exportAction;
	QAction *m_exitAction;

	GLWidget *m_opengl;
	AnimTimeline *m_anim_timeline;

	QString m_open_file;
	bool m_loaded;

	Mesh* m_mesh;
	FBXMesh* m_fbxmesh;
	VMFExport* m_vmf;

	AnimList* m_animlist;

private slots:
	void importMesh();
	bool exportMesh();
	void exitProgram();
};
