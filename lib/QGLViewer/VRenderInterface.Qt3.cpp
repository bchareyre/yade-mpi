/****************************************************************************
** Form implementation generated from reading ui file 'VRenderInterface.Qt3.ui'
**
** Created: Čt bře 13 19:58:46 2008
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "VRenderInterface.Qt3.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a VRenderInterface as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
VRenderInterface::VRenderInterface( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "VRenderInterface" );
    VRenderInterfaceLayout = new QVBoxLayout( this, 5, 6, "VRenderInterfaceLayout"); 

    includeHidden = new QCheckBox( this, "includeHidden" );
    VRenderInterfaceLayout->addWidget( includeHidden );

    cullBackFaces = new QCheckBox( this, "cullBackFaces" );
    VRenderInterfaceLayout->addWidget( cullBackFaces );

    blackAndWhite = new QCheckBox( this, "blackAndWhite" );
    VRenderInterfaceLayout->addWidget( blackAndWhite );

    colorBackground = new QCheckBox( this, "colorBackground" );
    VRenderInterfaceLayout->addWidget( colorBackground );

    tightenBBox = new QCheckBox( this, "tightenBBox" );
    VRenderInterfaceLayout->addWidget( tightenBBox );

    layout3 = new QHBoxLayout( 0, 0, 6, "layout3"); 

    sortLabel = new QLabel( this, "sortLabel" );
    layout3->addWidget( sortLabel );

    sortMethod = new QComboBox( FALSE, this, "sortMethod" );
    layout3->addWidget( sortMethod );
    VRenderInterfaceLayout->addLayout( layout3 );
    spacer1 = new QSpacerItem( 31, 41, QSizePolicy::Minimum, QSizePolicy::Expanding );
    VRenderInterfaceLayout->addItem( spacer1 );

    Layout4 = new QHBoxLayout( 0, 0, 6, "Layout4"); 

    SaveButton = new QPushButton( this, "SaveButton" );
    Layout4->addWidget( SaveButton );

    CancelButton = new QPushButton( this, "CancelButton" );
    Layout4->addWidget( CancelButton );
    VRenderInterfaceLayout->addLayout( Layout4 );
    languageChange();
    resize( QSize(298, 245).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( SaveButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( CancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );

    // tab order
    setTabOrder( SaveButton, CancelButton );
    setTabOrder( CancelButton, includeHidden );
    setTabOrder( includeHidden, cullBackFaces );
    setTabOrder( cullBackFaces, blackAndWhite );
    setTabOrder( blackAndWhite, colorBackground );
    setTabOrder( colorBackground, tightenBBox );
    setTabOrder( tightenBBox, sortMethod );
}

/*
 *  Destroys the object and frees any allocated resources
 */
VRenderInterface::~VRenderInterface()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void VRenderInterface::languageChange()
{
    setCaption( tr( "Vectorial rendering options" ) );
    includeHidden->setText( tr( "Include hidden parts" ) );
    QToolTip::add( includeHidden, tr( "Hidden poligons are also included in the output (usually twice bigger)" ) );
    cullBackFaces->setText( tr( "Cull back faces" ) );
    QToolTip::add( cullBackFaces, tr( "Back faces (non clockwise point ordering) are removed from the output" ) );
    blackAndWhite->setText( tr( "Black and white" ) );
    QToolTip::add( blackAndWhite, tr( "Black and white rendering" ) );
    colorBackground->setText( tr( "Color background" ) );
    QToolTip::add( colorBackground, tr( "Use current background color instead of white" ) );
    tightenBBox->setText( tr( "Tighten bounding box" ) );
    QToolTip::add( tightenBBox, tr( "Fit output bounding box to current display" ) );
    sortLabel->setText( tr( "Sort method :" ) );
    QToolTip::add( sortLabel, tr( "Polygon depth sorting method" ) );
    sortMethod->clear();
    sortMethod->insertItem( tr( "No sorting" ) );
    sortMethod->insertItem( tr( "BSP " ) );
    sortMethod->insertItem( tr( "Topological" ) );
    sortMethod->insertItem( tr( "Advanced topological" ) );
    sortMethod->setCurrentItem( 3 );
    QToolTip::add( sortMethod, tr( "Polygon depth sorting method" ) );
    SaveButton->setText( tr( "Save" ) );
    CancelButton->setText( tr( "Cancel" ) );
}

