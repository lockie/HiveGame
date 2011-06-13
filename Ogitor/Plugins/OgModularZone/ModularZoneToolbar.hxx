///Modular Zone Plugin
///
/// Copyright (c) 2009 Gary Mclean
//
//This program is free software; you can redistribute it and/or modify it under
//the terms of the GNU Lesser General Public License as published by the Free Software
//Foundation; either version 2 of the License, or (at your option) any later
//version.
//
//This program is distributed in the hope that it will be useful, but WITHOUT
//ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
//FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
//
//You should have received a copy of the GNU Lesser General Public License along with
//this program; if not, write to the Free Software Foundation, Inc., 59 Temple
//Place - Suite 330, Boston, MA 02111-1307, USA, or go to
//http://www.gnu.org/copyleft/lesser.txt.
////////////////////////////////////////////////////////////////////////////////*/
#pragma once

#include <QtGui/QWidget>
#include <QtGui/QToolBar>
#include <QtGui/QBoxLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QComboBox>
#include <QtGui/QAction>
#include <Ogre.h>
#include "ZoneListWidget.hxx"

namespace MZP
{

	
	class ModularZoneToolbar : public QWidget
	{
		Q_OBJECT;
	public:
		ModularZoneToolbar(QWidget *parent = 0);
		virtual ~ModularZoneToolbar(void);
		QToolBar* getToolbar(void){return mZoneSelectionToolbar;}

		//Slots
	public Q_SLOTS:
		void addZoneProperties(void);
		void createZone(void);

	protected:
		QToolBar* mZoneSelectionToolbar;

		ZoneListWidget* widget;

	};
}
//
//To avoid linking errors we need to specify Moc'ing
//GENERATE MOC FILES FOR HEADERS USING QT EVENTS (Q_OBJECT): 
//Perform this step for all header files that inherit from a QT class - i.e. any class with Q_OBJECT. The following example uses a ficticious header file called MyHeaderFile.h You must fully qualify where the header file is or the moc utility will not find it.
//
//In the Solution Explorer, right Click on MyHeaderFile.h and select Properties -> Custom Build Step -> General  
//Set Command Line to $(QTDIR)\bin\moc MyHeaderFile.h -o tmp\moc\moc_MyHeaderFile.cpp 
//Set Description to Moc'ing MyHeaderFile.h ... 
//Set Outputs to tmp\moc\moc_MyHeaderFile.cpp 
//Set Additional Dependences $(QTDIR)\bin\moc.exe; 
//Replace MyHeaderFile with the actual name of the header file to be Moc'd.  

