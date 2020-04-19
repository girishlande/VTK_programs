 ui->setupUi(this);

    // Create central widget
    centralwidget = new QWidget(this);
    centralwidget->setObjectName(QStringLiteral("centralwidget"));

    // Create layout for central widget
    vboxLayout = new QVBoxLayout(centralwidget);
    vboxLayout->setSpacing(1);
    vboxLayout->setObjectName(QStringLiteral("vboxLayout"));

    QWidget *firstPageWidget = new WidgetA;
    QWidget *secondPageWidget = new WidgetB;
    QWidget *thirdPageWidget = new WidgetC;

    QStackedWidget *stackedWidget = new QStackedWidget;
    stackedWidget->addWidget(firstPageWidget);
    stackedWidget->addWidget(secondPageWidget);
    stackedWidget->addWidget(thirdPageWidget);

    QComboBox *pageComboBox = new QComboBox;
    pageComboBox->addItem(tr("Page 1"));
    pageComboBox->addItem(tr("Page 2"));
    pageComboBox->addItem(tr("Page 3"));

    vboxLayout->addWidget(stackedWidget);
    vboxLayout->addWidget(pageComboBox);

    this->setCentralWidget(centralwidget);

    connect(pageComboBox, SIGNAL(activated(int)),
            stackedWidget, SLOT(setCurrentIndex(int)));