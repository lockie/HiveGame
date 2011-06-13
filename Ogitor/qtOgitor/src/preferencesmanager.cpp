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

#include <iostream>

#include <QtGui/QHBoxLayout>
#include <QtGui/QToolButton>
#include <QtGui/QPushButton>
#include <QtGui/QTabWidget>
#include <QtGui/QApplication>
#include <QtGui/QScrollArea>
#include <QtGui/QMessageBox>

#include "preferencesmanager.hxx"

//--------------------------------------------------------------------------------
PreferencesManager::PreferencesManager(QWidget *parent) : QObject(parent)
{
    mParentWidget = parent;
    createPreferencesDialog(parent);
    mCurrentSection = 0;
    mSectionCount = 0;

    mOgitorPrefWidget = new OgitorPreferencesWidget(parent);
    Ogre::NameValuePairList preferences;
    mOgitorPrefWidget->getPreferencesWidget(preferences);
    addCoreSection(tr("Ogitor"), ":/icons/qtOgitor.png", mOgitorPrefWidget);

    //mShortCutSettings = new ShortCutSettings(parent);
    //addCoreSection(tr("Controls"), ":/icons/controls.svg", mShortCutSettings);

    //mPluginsRootItem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(tr("Plugins")));
    //mPluginsRootItem->setIcon(0, QIcon(":/icons/additional.svg"));
    //mTreeWidgetRoot->addChild(mPluginsRootItem);

    setupSections();

    mTreeWidget->expandAll();

    connect(mTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));

    mPreferencesSections[tr("Ogitor")]->treeItem->setSelected(true);
}
//--------------------------------------------------------------------------------
PreferencesManager::~PreferencesManager()
{
    std::map<QString, PreferencesSection*>::iterator it = mPreferencesSections.begin();
    while(it != mPreferencesSections.end())
    {
        delete it->second;
        it++;
    }
    mPreferencesSections.clear();
}
//--------------------------------------------------------------------------------
void PreferencesManager::addCoreSection(QString identifier, QString sectionImagePath, QWidget *widget)
{
//     if(mButtonsLayout->findChild<QToolButton *>(identifier + "_button")) // already exists
//         return;
    
    QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(identifier));
    item->setIcon(0, QIcon(sectionImagePath));
    item->setWhatsThis(0, QString::number((qlonglong)widget));
    mTreeWidgetRoot->addChild(item);
    
    connect(widget, SIGNAL(isDirty()), this, SLOT(onIsDirty()));


    PreferencesSection *sec = new PreferencesSection();
    sec->identifier = identifier;
    sec->widget = widget;
    sec->treeItem = item;
    
    mCurrentSection = sec;
    mContentsLayout->addWidget(sec->widget);
    
    mPreferencesSections[identifier] = sec;
    mSectionCount++;
}
//--------------------------------------------------------------------------------
void PreferencesManager::addSection(QString identifier, QString sectionImagePath, QWidget *widget)
{
//     if(mButtonsLayout->findChild<QToolButton *>(identifier + "_button")) // already exists
//         return;
    QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(identifier));
    item->setIcon(0, QIcon(sectionImagePath));
    item->setWhatsThis(0, QString::number((qlonglong)widget));
    mPluginsRootItem->addChild(item);
    connect(widget, SIGNAL(isDirty()), this, SLOT(onIsDirty()));

    PreferencesSection *sec = new PreferencesSection();
    sec->identifier = identifier;
    sec->widget = widget;
    sec->treeItem = item;
        
    mCurrentSection = sec;
    mContentsLayout->addWidget(sec->widget);
                
    mPreferencesSections[identifier] = sec;
    mSectionCount++;
}
//--------------------------------------------------------------------------------
void PreferencesManager::createPreferencesDialog(QWidget *parent)
{
    mTreeWidget = new QTreeWidget();
    mTreeWidget->setHeaderHidden(true);
    mTreeWidget->setColumnCount(1);
    mTreeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    mTreeWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
    mTreeWidget->setMinimumWidth(160);
    mTreeWidget->setMaximumWidth(160);

    mTreeWidgetRoot = new QTreeWidgetItem((QTreeWidget*)0, QStringList(tr("Preferences")));
    mTreeWidgetRoot->setIcon(0, QIcon(":/icons/preferences.svg"));
    QFont fnt = mTreeWidgetRoot->font(0);
    fnt.setBold(true);
    mTreeWidgetRoot->setFont(0, fnt);
    mTreeWidget->addTopLevelItem(mTreeWidgetRoot);
    
    mBtnBox = new QDialogButtonBox();
    mBtnBox->centerButtons();
    QPushButton *applybutton = mBtnBox->addButton(QDialogButtonBox::Apply);
    mBtnBox->addButton(QDialogButtonBox::Ok);
    mBtnBox->addButton(QDialogButtonBox::Cancel);

    mPrefDlg = new QDialog(parent);
    mPrefDlg->setMinimumWidth(500);
    mPrefDlg->setMinimumHeight(300);

    QHBoxLayout *mMainLayout = new QHBoxLayout();
    mMainLayout->addWidget(mTreeWidget);

    QVBoxLayout *mainVerticalLayout = new QVBoxLayout();
    mContentsLayout = new QHBoxLayout();
    mainVerticalLayout->addLayout(mContentsLayout);
    mainVerticalLayout->addWidget(mBtnBox);
  
    mMainLayout->addLayout(mainVerticalLayout);
    mMainLayout->setStretch(0, 0);
    mMainLayout->setStretch(1, 1);

    connect(mBtnBox, SIGNAL(accepted()), this, SLOT(onAccept()));
    connect(mBtnBox, SIGNAL(rejected()), mPrefDlg, SLOT(reject()));
    connect(applybutton, SIGNAL(clicked()), this, SLOT(onApply()));
    applybutton->setEnabled(false);
    
    mPrefDlg->setLayout(mMainLayout);
    mPrefDlg->setMinimumSize(680, 350);

}
//--------------------------------------------------------------------------------
void PreferencesManager::setupSections()
{
    mPreferencesEditors = Ogitors::OgitorsRoot::getSingleton().GetPreferenceEditorList();

    QSettings settings;

    Ogitors::PreferenceEditorVector::iterator iter;
    for(iter=mPreferencesEditors.begin(); iter!=mPreferencesEditors.end(); iter++)
    {
        Ogitors::PreferenceEditorRegistrationStruct str = (Ogitors::PreferenceEditorRegistrationStruct)(*iter);
        QWidget *widg = (QWidget*)str.PrefEditor->getPreferencesWidget();
        settings.beginGroup(QString(str.Identifier.c_str()));
        QStringList keys = settings.allKeys();
        while (!keys.isEmpty())
        {
            QString key = keys.takeFirst();
            std::cout << "Key: " << key.toStdString() << std::endl;
            std::cout << "Value: " << settings.value(key).toString().toStdString() << std::endl;
        }
        addSection(QString(str.Identifier.c_str()), QString(str.ImagePath.c_str()), widg);
        settings.endGroup();
    }
}
//--------------------------------------------------------------------------------
void PreferencesManager::showDialog()
{
    mPrefDlg->exec();
}
//--------------------------------------------------------------------------------
void PreferencesManager::showSection(QString identifier)
{
    if(mPreferencesSections[identifier])
    {
        mContentsLayout->removeWidget(mCurrentSection->widget);
        mCurrentSection->widget->hide();
        mCurrentSection = mPreferencesSections[identifier];
        mContentsLayout->addWidget(mCurrentSection->widget);
        mCurrentSection->widget->show();

    }
}
//--------------------------------------------------------------------------------
void PreferencesManager::onApply()
{
    bool result = true;

    result &= mOgitorPrefWidget->applyPreferences();

    Ogitors::PreferenceEditorVector::iterator iter;
    for(iter=mPreferencesEditors.begin(); iter!=mPreferencesEditors.end(); iter++)
    {
        Ogitors::PreferenceEditorRegistrationStruct str = (Ogitors::PreferenceEditorRegistrationStruct)(*iter);
        result &= str.PrefEditor->applyPreferences();
    }

    mBtnBox->button(QDialogButtonBox::Apply)->setEnabled(!result); // if we reach here its safe to disable the button
}
//--------------------------------------------------------------------------------
void PreferencesManager::onAccept()
{
    if(mBtnBox->button(QDialogButtonBox::Apply)->isEnabled())
        onApply();

    savePreferences();

    mPrefDlg->accept();
}
//--------------------------------------------------------------------------------
void PreferencesManager::onIsDirty()
{
    mBtnBox->button(QDialogButtonBox::Apply)->setEnabled(true);
}
//--------------------------------------------------------------------------------
void PreferencesManager::selectionChanged()
{
    QList<QTreeWidgetItem*> list = mTreeWidget->selectedItems();
    if(list.size() == 0)
        return;

    if(list[0]->parent() == 0)
        return;
    
    QString name = list[0]->text(0);
    showSection(name);    
}
//--------------------------------------------------------------------------------
void PreferencesManager::savePreferences()
{
    ///////// Ogitors Preferences Widget //////////////////
    Ogre::NameValuePairList preferences;
    mOgitorPrefWidget->getPreferences(preferences);

    QSettings settings;
    settings.beginGroup("preferences");
    Ogre::NameValuePairList::const_iterator na;
    for(na=preferences.begin(); na!=preferences.end(); na++)
        settings.setValue(na->first.c_str(), na->second.c_str());
    settings.endGroup();

    ///////////// Plugin Widgets ///////////////////////////
    Ogitors::PreferenceEditorVector::iterator iter;
    for(iter=mPreferencesEditors.begin(); iter!=mPreferencesEditors.end(); iter++)
    {
        Ogre::NameValuePairList preferences;
        Ogitors::PreferenceEditorRegistrationStruct str = (Ogitors::PreferenceEditorRegistrationStruct)(*iter);
        str.PrefEditor->getPreferences(preferences);

        Ogre::NameValuePairList::const_iterator ni;

        settings.beginGroup(str.Identifier.c_str());

        for(ni=preferences.begin(); ni!=preferences.end(); ni++)
        {
            settings.setValue(ni->first.c_str(), ni->second.c_str());
        }

        settings.endGroup();

    }
}
//--------------------------------------------------------------------------------
