/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2011 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
//
/// The MIT License
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////*/

#include <QtGui/QMenu>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtGui/QPainter>
#include <QtGui/QColorDialog>
#include <QtCore/QEvent>
#include <QtCore/QDirIterator>
#include <QtGui/QDragEnterEvent>
#include <QtCore/QUrl>
#include "settingsdialog.hxx"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "BaseEditor.h"
#include "ViewGrid.h"

using namespace Ogitors;

const int RES_LOC_DIR = 1;
const int RES_LOC_ZIP = 2;


extern QString ConvertToQString(Ogre::UTFString& value);
//----------------------------------------------------------------------------------
SettingsDialog::SettingsDialog(QWidget *parent, PROJECTOPTIONS *options) :
QDialog(parent, Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint)
{
   setupUi(this);

   connect(buttonBox, SIGNAL(accepted()), this, SLOT(onAccept()));
   connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
   connect(mBrowseProjectDirButton, SIGNAL(clicked()), this, SLOT(browse()));

   mOptions = options;

   mProjectDirTextBox->setText(mOptions->ProjectDir.c_str());
   mProjectNameTextBox->setText(mOptions->ProjectName.c_str());
   mSceneMgrNameMenu->addItem("OctreeSceneManager");
   mSceneMgrNameMenu->setCurrentIndex(0);
   mConfigFileTextBox->setText(mOptions->SceneManagerConfigFile.c_str());
   mTerrainDirTextBox->setText(mOptions->TerrainDirectory.c_str());

   if(!mOptions->IsNewProject)
   {
      mProjectNameTextBox->setText(QApplication::translate("QtOgitorSystem", "<Enter New Name>"));         
      mProjectDirTextBox->setEnabled(false);
      mProjectNameTextBox->setEnabled(false);
      mSceneMgrNameMenu->setEnabled(false);
      mConfigFileTextBox->setEnabled(false);
      mTerrainDirTextBox->setEnabled(false);
      mBrowseProjectDirButton->setEnabled(false);
   }

   unsigned int i;
   QString value;
   for(i = 0;i < mOptions->ResourceDirectories.size();i++)
   {
      value = mOptions->ResourceDirectories[i].c_str();
      value = value.right(value.length() - 3);
      if(mOptions->ResourceDirectories[i].find("FS:") == 0)
      {
         addResourceLocation(RES_LOC_DIR, value);
      }
      else if(mOptions->ResourceDirectories[i].find("ZP:") == 0)
      {
         addResourceLocation(RES_LOC_ZIP, value);
      }
   }
   mResourceListBox->installEventFilter(this);

   mSelRectColourWidget = new ColourPickerWidget(this, QColor(mOptions->SelectionRectColour.r * 255, mOptions->SelectionRectColour.g * 255, mOptions->SelectionRectColour.b * 255));   
   mSelColourWidget = new ColourPickerWidget(this, QColor(mOptions->SelectionBBColour.r * 255, mOptions->SelectionBBColour.g * 255, mOptions->SelectionBBColour.b * 255));   
   mHighlightColourWidget = new ColourPickerWidget(this, QColor(mOptions->HighlightBBColour.r * 255, mOptions->HighlightBBColour.g * 255, mOptions->HighlightBBColour.b * 255));   
   mSelectHighlightColourWidget = new ColourPickerWidget(this, QColor(mOptions->SelectHighlightBBColour.r * 255, mOptions->SelectHighlightBBColour.g * 255, mOptions->SelectHighlightBBColour.b * 255));   
   mSelectionGridLayout->addWidget(mSelRectColourWidget,0,1,1,1);
   mSelectionGridLayout->addWidget(mSelColourWidget,1,1,1,1);
   mSelectionGridLayout->addWidget(mHighlightColourWidget,2,1,1,1);
   mSelectionGridLayout->addWidget(mSelectHighlightColourWidget,3,1,1,1);

   mGridColourWidget = new ColourPickerWidget(this, QColor(mOptions->GridColour.r * 255, mOptions->GridColour.g * 255, mOptions->GridColour.b * 255));   
   mGridAppearanceLayout->addWidget(mGridColourWidget,1,1,1,1);

   mGridSpacingMenu->setValue(ViewportGrid::getGridSpacing());
   mSnapAngleMenu->setValue(0);
   mSelectionDepthMenu->setValue(mOptions->VolumeSelectionDepth);

   mSelRectColourWidget->setMaximumSize(40,20);
   mSelRectColourWidget->setMinimumSize(40,20);
   mSelColourWidget->setMaximumSize(40,20);
   mSelColourWidget->setMinimumSize(40,20);
   mHighlightColourWidget->setMaximumSize(40,20);
   mHighlightColourWidget->setMinimumSize(40,20);
   mSelectHighlightColourWidget->setMaximumSize(40,20);
   mSelectHighlightColourWidget->setMinimumSize(40,20);
   mGridColourWidget->setMaximumSize(40,20);
   mGridColourWidget->setMinimumSize(40,20);

   tabWidget->setCurrentIndex(0);

   // Enable dropping on the widget
   setAcceptDrops(true);
}
//----------------------------------------------------------------------------------
SettingsDialog::~SettingsDialog()
{
}
//----------------------------------------------------------------------------------
void SettingsDialog::browse()
{
   QString path = QFileDialog::getExistingDirectory(QApplication::activeWindow(), "",QApplication::applicationDirPath()
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
      , QFileDialog::DontUseNativeDialog | QFileDialog::ShowDirsOnly);
#else
      );
#endif
   if(!path.isEmpty())
      mProjectDirTextBox->setText(path);
}
//----------------------------------------------------------------------------------
bool IsValidName(QString strName, QString strDisplayName, QString exkeys = "")
{
   strName = strName.trimmed();
   bool found = false;
   QString mask = "%\"<>#,?&;" + exkeys;
   for(int i = 0;i < mask.size();i++)
   {
      if(strName.contains(mask[i]))
         found = true;
   }

   if(found)
   {
      Ogre::UTFString msgTemp = OgitorsSystem::getSingletonPtr()->Translate("%1 can not contain (\"<>,\"#?&;%2\")");

      QString msg = ConvertToQString(msgTemp).arg(strDisplayName).arg(exkeys);
      QMessageBox::warning(QApplication::activeWindow(),"qtOgitor", msg, QMessageBox::Ok);
      return false;
   }
   if(strName.isEmpty())
   {
      Ogre::UTFString msgTemp = OgitorsSystem::getSingletonPtr()->Translate("%1 does not contain a valid value!");

      QString msg = ConvertToQString(msgTemp).arg(strDisplayName);
      QMessageBox::warning(QApplication::activeWindow(),"qtOgitor", msg, QMessageBox::Ok);
      return false;
   }
   return true;
}
//----------------------------------------------------------------------------------
QString lastDirPath = "";

void SettingsDialog::addResourceLocation(int loctype, QString path)
{
   bool found = false;

   for(int i = 0;i < mResourceListBox->count();i++)
   {
      if(mResourceListBox->item(i)->text() == path)
      {
         found = true;
         break;
      }
   }

   if(!found)
   {
      mResourceListBox->addItem(path);
      mResourceFileTypes.push_back(loctype);
   }
}
//----------------------------------------------------------------------------------
void SettingsDialog::onAddDirectory()
{
   QString path;
   if(lastDirPath == "")    
      path = QApplication::applicationDirPath();
   else
      path = lastDirPath;

   path = QFileDialog::getExistingDirectory(QApplication::activeWindow(), "", path
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
      , QFileDialog::DontUseNativeDialog | QFileDialog::ShowDirsOnly);
#else
      );
#endif
   if(!path.isEmpty())
   {
      addResourceLocation(RES_LOC_DIR, path);
      lastDirPath = path;
   }
}

//----------------------------------------------------------------------------------
void SettingsDialog::onAddDirectoryRecursive()
{
   QString path;
   if(lastDirPath == "")    
      path = QApplication::applicationDirPath();
   else
      path = lastDirPath;

   path = QFileDialog::getExistingDirectory(QApplication::activeWindow(), "", path
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
      , QFileDialog::DontUseNativeDialog | QFileDialog::ShowDirsOnly);
#else
      );
#endif

   // DO NOT MAKE RELATIVE CONVERSION HERE, IT WILL BE DONE WHEN OK IS CLICKED, 
   // THE PROJECT DIRECTORY MAY NOT BE DETERMINED AT THIS POINT
   if(!path.isEmpty())
   {
      lastDirPath = path;

      addResourceLocation(RES_LOC_DIR, path);

      QDirIterator it(path, QDir::AllDirs, QDirIterator::Subdirectories);
      while (it.hasNext()) 
      {
         QString dir = it.next();        
         QFileInfo inf(dir);

         if(!dir.endsWith("."))
         {
            addResourceLocation(RES_LOC_DIR, dir);
         }
      }
   }
}

//----------------------------------------------------------------------------------
void SettingsDialog::onAddArchive()
{
   QString path;
   if(lastDirPath == "")    
      path = QApplication::applicationDirPath();
   else
      path = lastDirPath;

   path = QFileDialog::getOpenFileName(QApplication::activeWindow(), tr("Archive Files"), path, QString(tr("Zip Files (*.zip)")), 0
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
      , QFileDialog::DontUseNativeDialog);
#else
      );
#endif
   if(!path.isEmpty())
   {
      addResourceLocation(RES_LOC_ZIP, path);
   }
}
//----------------------------------------------------------------------------------
void SettingsDialog::onRemoveEntry()
{
   if(mResourceListBox->currentIndex().isValid())
   {
      int index = mResourceListBox->currentIndex().row();
      if(index >= 0)
      {
         mResourceFileTypes.erase(mResourceFileTypes.begin() + index);
         mResourceListBox->takeItem(index);
      }
   }
}
//----------------------------------------------------------------------------------
bool SettingsDialog::eventFilter ( QObject * watched, QEvent * e )
{
    if(watched == mResourceListBox)
    {
        if(e->type() == QEvent::ContextMenu)
        {
            QMenu *menu = new QMenu(this);
            menu->addAction(tr("Add Directory"), this, SLOT(onAddDirectory()));
            menu->addAction(tr("Add Directories Recursively"), this, SLOT(onAddDirectoryRecursive()));
            menu->addAction(tr("Add Archive"), this, SLOT(onAddArchive()));
            QAction *removeAction = menu->addAction(tr("Remove Entry"), this, SLOT(onRemoveEntry()));
            removeAction->setEnabled(mResourceListBox->currentIndex().row() >= 0);

            menu->exec(QCursor::pos());
            delete menu;
            e->ignore();
            return true;
        }
        else if(e->type() == QEvent::KeyRelease)
        {
            QKeyEvent *evt = static_cast<QKeyEvent*>(e);

            if(evt->key() == Qt::Key_Delete)
            {
                onRemoveEntry();
                return true;
            }
        }
    }
    return false;
}
//----------------------------------------------------------------------------------
void SettingsDialog::onAccept()
{
   if(mOptions->IsNewProject)
   {
      QString ProjectDir = mProjectDirTextBox->text();
      QString ProjectName = mProjectNameTextBox->text();
      QString SceneManagerName = mSceneMgrNameMenu->itemText(mSceneMgrNameMenu->currentIndex());
      QString SceneManagerConfigFile = mConfigFileTextBox->text();
      QString TerrainDir = mTerrainDirTextBox->text();

      if(!IsValidName(ProjectDir, qApp->translate("SettingsDialog", "Project Directory")))
         return;
      if(!IsValidName(ProjectName, qApp->translate("SettingsDialog", "Project Name"), "\\/"))
         return;
      if(!IsValidName(TerrainDir, qApp->translate("SettingsDialog", "Terrain Directory")))
         return;

      Ogre::String sProjectDir = OgitorsUtils::QualifyPath(QString(ProjectDir + QString("/") + ProjectName).toStdString());

      OgitorsSystem::getSingletonPtr()->MakeDirectory(sProjectDir);

      mOptions->CreatedIn = ""; 
      mOptions->ProjectDir = sProjectDir;
      mOptions->ProjectName = ProjectName.toStdString();
      mOptions->SceneManagerName = SceneManagerName.toStdString();
      mOptions->SceneManagerConfigFile = SceneManagerConfigFile.toStdString();
      mOptions->TerrainDirectory = TerrainDir.toStdString();
   }

   mOptions->ResourceDirectories.clear();

   Ogre::String pathTo = mOptions->ProjectDir;

   HashMap<Ogre::String, int> resDirMap;

   unsigned int i;
   unsigned int itemcount = mResourceListBox->count();
   for(i = 0;i < itemcount;i++)
   { 
      Ogre::String strTemp = mResourceListBox->item(i)->text().toStdString();
      int stype = mResourceFileTypes[i];
      if(strTemp.substr(0,1) != ".")
         strTemp = OgitorsUtils::GetRelativePath(pathTo,strTemp);

      Ogre::String val;
      if(stype == RES_LOC_DIR)
      {
         val = Ogre::String("FS:") + strTemp;
         if(resDirMap.find(val) == resDirMap.end())
         {
            resDirMap[val] = 0;
         }
      }
      else if(stype == RES_LOC_ZIP)
      {   
         val = Ogre::String("ZP:") + strTemp;
         if(resDirMap.find(val) == resDirMap.end())
         {
            resDirMap[val] = 0;
         }
      }
   }

   HashMap<Ogre::String, int>::const_iterator rit = resDirMap.begin();
   while(rit != resDirMap.end())
   {
      mOptions->ResourceDirectories.push_back(rit->first);
      rit++;
   }

   mOptions->SelectionRectColour = mSelRectColourWidget->getColour();
   mOptions->SelectionBBColour = mSelColourWidget->getColour();
   mOptions->HighlightBBColour = mHighlightColourWidget->getColour();
   mOptions->SelectHighlightBBColour = mSelectHighlightColourWidget->getColour();
   mOptions->GridColour = mGridColourWidget->getColour();
   mOptions->GridSpacing = mGridSpacingMenu->value();
   mOptions->SnapAngle = mSnapAngleMenu->value();
   mOptions->VolumeSelectionDepth = mSelectionDepthMenu->value();

   accept();
}
//----------------------------------------------------------------------------------
void SettingsDialog::dragEnterEvent(QDragEnterEvent * e)
{
   // Only accept drags on the resources tab
   if(tabWidget->currentIndex() != 1)
   {
      e->ignore();
      return;
   }

   // Get the filenames
   QStringList filenames = getFilenames(e->mimeData());

   // Don't accept empty drags
   if(filenames.empty())
   {
      e->ignore();
      return;
   }

   // Accept when only directories and zip-files are being dragged
   for(int i = 0; i < filenames.size(); ++i)
   {
      QFileInfo file(filenames.at(i));
      QString extension = file.suffix().toLower();

      if(!file.isDir() && extension != "zip")
      {
         e->ignore();
         return;
      }
   }
   e->accept();
}
//----------------------------------------------------------------------------------
void SettingsDialog::dropEvent(QDropEvent * e)
{
   // This should only occur when the resource tab is active
   assert(tabWidget->currentIndex() == 1);

   // Get the dropped filenames
   QStringList filenames = getFilenames(e->mimeData());

   if(filenames.empty())
      e->ignore();

   // Handle the dropped items
   for(int i = 0; i < filenames.size(); ++i)
   {
      // All dropped items should be directories
      QFileInfo file(filenames.at(i));
      QString extension = file.suffix().toLower();

      if(file.isDir())
      {
         addResourceLocation(RES_LOC_DIR, filenames.at(i));
      }
      else if(extension == "zip")
      {
         addResourceLocation(RES_LOC_ZIP, filenames.at(i));
      }
   }
}
//----------------------------------------------------------------------------------
QStringList SettingsDialog::getFilenames(const QMimeData * data)
{
   QStringList result;

   QList<QUrl> urls = data->urls();
   for(int i = 0; i < urls.size(); ++i)
      result.push_back(urls.at(i).toLocalFile());

   return result;
}
//----------------------------------------------------------------------------------
