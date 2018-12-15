#include "tcc_designer.h"
#include "../mainwindow.h"
#include "ui_tcc_designer.h"

TccDesigner::TccDesigner(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TccDesigner),
    m_pButtonGroupAlignment(new QButtonGroup),
    m_pPyListMenu(nullptr),
    m_pRegExpValidator(new QRegExpValidator(QRegExp("(\\d?[a-f]?[A-F]?){0,6}"), this))
{
    ui->setupUi(this);

    this->addCustomActions();

    ui->comboBoxKaraokeMode->addItems(QStringList() << TCC_K_MODE_NORMAL << TCC_K_MODE_SYLLABLE << TCC_K_MODE_ADVANCED);
    ui->comboBoxTextLayout->addItems(QStringList() << TCC_TEXT_LAYOUT_HOR << TCC_TEXT_LAYOUT_VER);

    USING SCOPE
    {
        const QList<QLineEdit *> c_pLineEditColor = {
            ui->editPrimaryColor,
            ui->editSecondaryColor,
            ui->editBordColor,
            ui->editShadowColor,
        };

        for(int i = 0; i < c_pLineEditColor.length(); i++)
        {
            c_pLineEditColor.at(i)->setValidator(m_pRegExpValidator);
            c_pLineEditColor.at(i)->setText(DEFAULT_COLOR);
        }
    }END_OF_USING;

    USING SCOPE
    {
        const QList<QAbstractButton *> c_pButtonAlignment = {
            ui->buttonAlignment1,
            ui->buttonAlignment2,
            ui->buttonAlignment3,
            ui->buttonAlignment4,
            ui->buttonAlignment5,
            ui->buttonAlignment6,
            ui->buttonAlignment7,
            ui->buttonAlignment8,
            ui->buttonAlignment9,
        };

        for(int i = 0; i < c_pButtonAlignment.length(); i++)
        {
            c_pButtonAlignment.at(i)->setCheckable(true);
            c_pButtonAlignment.at(i)->setProperty(TCC_PROP_ALIGNMENT, i + TCC_INDEX_OFFSET);
            m_pButtonGroupAlignment->addButton(c_pButtonAlignment.at(i));
            if(i == TCC_ALIGNMENT_DEFAULT)
            {
                c_pButtonAlignment.at(i)->setChecked(true);
            }
        }
    }END_OF_USING;

    ui->comboBoxFontName->addItems(TcaxHelper::fontList());
    ui->comboBoxFontName->setCurrentIndex(-1);

    connect(ui->editPrimaryColor, SIGNAL(textChanged(QString)), this, SLOT(colorChanged(QString)));
    connect(ui->editSecondaryColor, SIGNAL(textChanged(QString)), this, SLOT(colorChanged(QString)));
    connect(ui->editBordColor, SIGNAL(textChanged(QString)), this, SLOT(colorChanged(QString)));
    connect(ui->editShadowColor, SIGNAL(textChanged(QString)), this, SLOT(colorChanged(QString)));
    connect(ui->buttonPrimaryColor, SIGNAL(clicked()), this, SLOT(selectColor()));
    connect(ui->buttonSecondaryColor, SIGNAL(clicked()), this, SLOT(selectColor()));
    connect(ui->buttonBordColor, SIGNAL(clicked()), this, SLOT(selectColor()));
    connect(ui->buttonShadowColor, SIGNAL(clicked()), this, SLOT(selectColor()));
}

TccDesigner::~TccDesigner()
{
    delete ui;
    delete m_pButtonGroupAlignment;
    delete m_pPyListMenu;
}

void TccDesigner::reload(const QString &a_filename)
{
#ifdef TCC_LIB_NEO
    TccMap tccMap = TcaxHelper::tccParseToMap(a_filename);

    setTccMap(tccMap);
#else
    TCC_Attributes attr;

    attr = TcaxHelper::tccParse(a_filename);
    setTccAttr(attr);
    TcaxHelper::tccFree(&attr);
#endif
}

bool TccDesigner::save(const QString &a_filename)
{
#ifdef TCC_LIB_NEO
    TccMap tccMap = getTccMap();

    return TcaxHelper::tccSave(a_filename, tccMap);
#else
    TCC_Attributes attr = getTccAttr();
    bool ret = false;

    ret = TcaxHelper::tccSave(a_filename, attr);
    freeTccAttr(&attr);
    return ret;
#endif
}

bool TccDesigner::isChanged(void)
{
    return true;
}

void TccDesigner::addCustomActions(void)
{
    /* PyListMenu */
    QJsonObject json;
    QListWidget *action_parent = ui->listWidgetLine;
    QList<QPair<QString, QAction *>> action_list = {
        ACTION_NEW(TCC_PY_LIST_MENU_DELETE, ":/buttons/cross_grey.png", action_parent),
        ACTION_NONE,
        ACTION_NEW(TCC_PY_LIST_MENU_MOVE_UP, ":/buttons/arrow_up.png", action_parent),
        ACTION_NEW(TCC_PY_LIST_MENU_MOVE_DOWN, ":/buttons/arrow_down.png", action_parent),
    };

    m_pPyListMenu = new QMenu(action_parent);

    for(int i = 0; i < action_list.length(); i++)
    {
        QString key = action_list.at(i).first;
        QAction *value = action_list.at(i).second;

        if( (key == NULLSTR) || (value == nullptr) )
        {
            m_pPyListMenu->addSeparator();
        }
        else
        {
            m_pPyListMenu->addAction(value);
            value->setProperty(ACTION_PROP_ID, key);
            value->connect(value, SIGNAL(triggered()), this, SLOT(lineChanged()));
        }
    }
    ui->buttonAddLine->setProperty(ACTION_PROP_ID, TCC_PY_LIST_MENU_ADD);
    connect(ui->buttonAddLine, SIGNAL(clicked()), this, SLOT(lineChanged()));
}

void TccDesigner::colorChanged(QString a_colorString)
{
#if false
    QPalette palette;

    palette.setColor(QPalette::Text, QColor(TCC_COLOR_INIT_MOJI + a_colorString));
    static_cast<QLineEdit *>(sender())->setPalette(palette);
#endif

    if(a_colorString.length() < eINDEX_6)
    {
        return;
    }

    if(sender() == ui->editPrimaryColor)
    {
        ui->buttonPrimaryColor->setStyleSheet(QString(buttonColorSelected).arg(a_colorString));
    }
    else if(sender() == ui->editSecondaryColor)
    {
        ui->buttonSecondaryColor->setStyleSheet(QString(buttonColorSelected).arg(a_colorString));
    }
    else if(sender() == ui->editBordColor)
    {
        ui->buttonBordColor->setStyleSheet(QString(buttonColorSelected).arg(a_colorString));
    }
    else if(sender() == ui->editShadowColor)
    {
        ui->buttonShadowColor->setStyleSheet(QString(buttonColorSelected).arg(a_colorString));
    }
}

void TccDesigner::freeTccAttr(TCC_pAttributes a_pTccAttr)
{
    free(a_pTccAttr->py_file);
    free(a_pTccAttr->beg_line);
    free(a_pTccAttr->end_line);
}

void TccDesigner::setTccMap(TccMap a_tccMap)
{
    /* Source */
    QJsonArray jsonArray;
    TccPyFileList tccPyFileList = TcaxHelper::tccMapParseToTccPyFileList(a_tccMap);

    ui->editSource->setText(a_tccMap.value(TCC_K_TIMED_ASS_FILE).toString());
    for(TccPyFileList::iterator i = tccPyFileList.begin(); i != tccPyFileList.end(); i++)
    {
        QJsonObject json;

        json.insert(TCC_JSON_PY_FILE, i.key());
        json.insert(TCC_JSON_BEG_LINE, i.value().first.toInt());
        json.insert(TCC_JSON_END_LINE, i.value().second.toInt());
        jsonArray.append(json);
    }
    setPythonLines(jsonArray);

    /* Effect */
    QString karaokeMode = a_tccMap.value(TCC_K_MODE).toString();
    QString textLayout = a_tccMap.value(TCC_TEXT_LAYOUT).toString();

    if(karaokeMode.startsWith(TCC_ADV))
    {
        karaokeMode = TCC_ADVANCED;
    }
    else if(karaokeMode.startsWith(TCC_SYL))
    {
        karaokeMode = TCC_SYLLABLE;
    }
    else
    {
        karaokeMode = TCC_NORMAL;
    }

    if(textLayout.startsWith(TCC_VERT))
    {
        textLayout = TCC_VERTICAL;
    }
    else if(textLayout.startsWith(TCC_HORI) || true)
    {
        textLayout = TCC_HORIZONTAL;
    }

    ui->comboBoxKaraokeMode->setCurrentText(karaokeMode);
    ui->comboBoxTextLayout->setCurrentText(textLayout);
    ui->checkBoxUsingTcaxPyInit->setChecked(a_tccMap.value(TCC_TCAXPY_INIT).toBool());
    ui->checkBoxUsingTcaxPyFin->setChecked(a_tccMap.value(TCC_TCAXPY_FIN).toBool());
    ui->checkBoxUsingTcaxPyUser->setChecked(a_tccMap.value(TCC_TCAXPY_USER).toBool());
    ui->checkBoxUsingSourceHeader->setChecked(a_tccMap.value(TCC_INHERIT_ASS_HEADER).toBool());

    /* Color */
    ui->doubleSpinBoxBlur->setValue(a_tccMap.value(TCC_BLUR).toDouble());
    ui->doubleSpinBoxSpaceScale->setValue(a_tccMap.value(TCC_SPACE_SCALE).toDouble());
    ui->spinBoxBord->setValue(a_tccMap.value(TCC_BORD).toInt());
    ui->spinBoxShadow->setValue(a_tccMap.value(TCC_SHAD).toInt());

    ui->editPrimaryColor->setText(a_tccMap.value(TCC_PRIMARY_COLOR).toString());
    ui->editSecondaryColor->setText(a_tccMap.value(TCC_SECONDARY_COLOR).toString());
    ui->editBordColor->setText(a_tccMap.value(TCC_OUTLINE_COLOR).toString());
    ui->editShadowColor->setText(a_tccMap.value(TCC_BACK_COLOR).toString());

    ui->spinBoxPrimaryColorAlpha->setValue(a_tccMap.value(TCC_PRIMARY_ALPHA).toInt());
    ui->spinBoxSecondaryColorAlpha->setValue(a_tccMap.value(TCC_SECONDARY_ALPHA).toInt());
    ui->spinBoxBordColorAlpha->setValue(a_tccMap.value(TCC_OUTLINE_ALPHA).toInt());
    ui->spinBoxShadowColorAlpha->setValue(a_tccMap.value(TCC_BACK_ALPHA).toInt());

    /* Alignment */
    const QList<QAbstractButton *> c_pButtonAlignment = m_pButtonGroupAlignment->buttons();

    for(int i = 0; i < c_pButtonAlignment.length(); i++)
    {
        if(c_pButtonAlignment.at(i)->property(TCC_PROP_ALIGNMENT) == a_tccMap.value(TCC_ALIGNMENT).toInt())
        {
            c_pButtonAlignment.at(i)->setChecked(true);
            break;
        }
    }

    /* Font */
    ui->comboBoxFontName->setCurrentText(a_tccMap.value(TCC_FONT_FACE_NAME).toString());
    ui->comboBoxFontNum->clear();
    ui->comboBoxFontNum->setCurrentText(a_tccMap.value(TCC_FONT_FACE_ID).toString());
    ui->editFontFile->setText(a_tccMap.value(TCC_FONT_FILE).toString());
    ui->spinBoxFontSize->setValue(a_tccMap.value(TCC_FONT_SIZE).toInt());

    /* Margins */
    ui->spinBoxOffsetX->setValue(a_tccMap.value(TCC_X_OFFSET).toInt());
    ui->spinBoxOffsetY->setValue(a_tccMap.value(TCC_Y_OFFSET).toInt());
    ui->spinBoxSpacing->setValue(a_tccMap.value(TCC_SPACING).toInt());
    ui->spinBoxWidth->setValue(a_tccMap.value(TCC_FX_WIDTH).toInt());
    ui->spinBoxHeight->setValue(a_tccMap.value(TCC_FX_HEIGHT).toInt());
    ui->doubleSpinBoxFPS->setValue(a_tccMap.value(TCC_FX_FPS).toDouble());
}

TccMap TccDesigner::getTccMap(void)
{
    TccMap tccMap;

    /* Source */
    QJsonArray jsonArray = Common::toJsonArray(ui->listWidgetLine->property(TCC_PY_LIST_JSON_ARRAY_PROP).toString());

    tccMap.insert(TCC_K_TIMED_ASS_FILE, ui->editSource->text());

    for(int i = 0; i < jsonArray.count(); i++)
    {
        QJsonObject json = jsonArray.at(i).toObject();
        QString pyFile = json[TCC_JSON_PY_FILE].toString();
        int begLine = json[TCC_JSON_BEG_LINE].toInt();
        int endLine = json[TCC_JSON_END_LINE].toInt();

        tccMap.insert(TCC_PY_FILE, pyFile);
        tccMap.insert(TCC_BEG_LINE, begLine);
        tccMap.insert(TCC_END_LINE, endLine);
    }

    /* Effect */
    tccMap.insert(TCC_K_MODE, ui->comboBoxKaraokeMode->currentText());
    tccMap.insert(TCC_TEXT_LAYOUT, ui->comboBoxTextLayout->currentText());
    tccMap.insert(TCC_TCAXPY_INIT, ui->checkBoxUsingTcaxPyInit->isChecked());
    tccMap.insert(TCC_TCAXPY_FIN, ui->checkBoxUsingTcaxPyFin->isChecked());
    tccMap.insert(TCC_TCAXPY_USER, ui->checkBoxUsingTcaxPyUser->isChecked());
    tccMap.insert(TCC_INHERIT_ASS_HEADER, ui->checkBoxUsingSourceHeader->isChecked());

    /* Color */
    tccMap.insert(TCC_BLUR, ui->doubleSpinBoxBlur->value());
    tccMap.insert(TCC_SPACE_SCALE, ui->doubleSpinBoxSpaceScale->value());
    tccMap.insert(TCC_BORD, ui->spinBoxBord->value());
    tccMap.insert(TCC_SHAD, ui->spinBoxShadow->value());

    tccMap.insert(TCC_PRIMARY_COLOR, ui->editPrimaryColor->text());
    tccMap.insert(TCC_SECONDARY_COLOR, ui->editSecondaryColor->text());
    tccMap.insert(TCC_OUTLINE_COLOR, ui->editBordColor->text());
    tccMap.insert(TCC_BACK_COLOR, ui->editShadowColor->text());

    tccMap.insert(TCC_PRIMARY_ALPHA, ui->spinBoxPrimaryColorAlpha->value());
    tccMap.insert(TCC_SECONDARY_ALPHA, ui->spinBoxSecondaryColorAlpha->value());
    tccMap.insert(TCC_OUTLINE_ALPHA, ui->spinBoxBordColorAlpha->value());
    tccMap.insert(TCC_BACK_ALPHA, ui->spinBoxShadowColorAlpha->value());

    /* Alignment */
    tccMap.insert(TCC_ALIGNMENT, m_pButtonGroupAlignment->checkedButton()->property(TCC_PROP_ALIGNMENT).toInt());

    /* Font */
    //tccMap.insert(TCC_FONT_FACE_NAME, ui->comboBoxFontName->currentText());
    tccMap.insert(TCC_FONT_FACE_NAME, NULLSTR);
    if (ui->comboBoxFontNum->count() <= eINDEX_0)
    {
        tccMap.insert(TCC_FONT_FACE_ID, ui->comboBoxFontNum->currentText().toInt());
    }
    else
    {
        tccMap.insert(TCC_FONT_FACE_ID, ui->comboBoxFontNum->currentIndex() + TCC_INDEX_OFFSET);
    }
    tccMap.insert(TCC_FONT_FILE, ui->editFontFile->text());
    tccMap.insert(TCC_FONT_SIZE, ui->spinBoxFontSize->value());

    /* Margins */
    tccMap.insert(TCC_X_OFFSET, ui->spinBoxOffsetX->value());
    tccMap.insert(TCC_Y_OFFSET, ui->spinBoxOffsetY->value());
    tccMap.insert(TCC_SPACING, ui->spinBoxSpacing->value());
    tccMap.insert(TCC_FX_WIDTH, ui->spinBoxWidth->value());
    tccMap.insert(TCC_FX_HEIGHT, ui->spinBoxHeight->value());
    tccMap.insert(TCC_FX_FPS, ui->doubleSpinBoxFPS->value());
    return tccMap;
}

void TccDesigner::setTccAttr(TCC_Attributes a_tccAttr)
{
    /* Source */
    QJsonArray jsonArray;

    ui->editSource->setText(Common::fromWCharT(a_tccAttr.k_timed_ass_file));
    for(int i = 0; i < a_tccAttr.py_file_count; i++)
    {
        QJsonObject json;
        wchar_t *py_file = a_tccAttr.py_file[i];
        int beg_line = a_tccAttr.beg_line[i];
        int end_line = a_tccAttr.end_line[i];

        json.insert(TCC_JSON_PY_FILE, Common::fromWCharT(py_file));
        json.insert(TCC_JSON_BEG_LINE, beg_line);
        json.insert(TCC_JSON_END_LINE, end_line);
        jsonArray.append(json);
    }
    setPythonLines(jsonArray);

    /* Effect */
    ui->comboBoxKaraokeMode->setCurrentIndex(a_tccAttr.k_mode - TCC_INDEX_OFFSET);
    ui->comboBoxTextLayout->setCurrentIndex(a_tccAttr.text_layout - TCC_INDEX_OFFSET);
    ui->checkBoxUsingTcaxPyInit->setChecked(static_cast<bool>(a_tccAttr.tcaxpy_init));
    ui->checkBoxUsingTcaxPyFin->setChecked(static_cast<bool>(a_tccAttr.tcaxpy_fin));
    ui->checkBoxUsingTcaxPyUser->setChecked(static_cast<bool>(a_tccAttr.tcaxpy_user));
    ui->checkBoxUsingSourceHeader->setChecked(static_cast<bool>(a_tccAttr.inherit_ass_header));

    /* Color */
    ui->doubleSpinBoxBlur->setValue(a_tccAttr.blur);
    ui->doubleSpinBoxSpaceScale->setValue(a_tccAttr.space_scale);
    ui->spinBoxBord->setValue(a_tccAttr.bord);
    ui->spinBoxShadow->setValue(a_tccAttr.shad);

    ui->editPrimaryColor->setText(Common::fromWCharT(a_tccAttr.primary_color));
    ui->editSecondaryColor->setText(Common::fromWCharT(a_tccAttr.secondary_color));
    ui->editBordColor->setText(Common::fromWCharT(a_tccAttr.outline_color));
    ui->editShadowColor->setText(Common::fromWCharT(a_tccAttr.back_color));

    ui->spinBoxPrimaryColorAlpha->setValue(a_tccAttr.primary_alpha);
    ui->spinBoxSecondaryColorAlpha->setValue(a_tccAttr.secondary_alpha);
    ui->spinBoxBordColorAlpha->setValue(a_tccAttr.outline_alpha);
    ui->spinBoxShadowColorAlpha->setValue(a_tccAttr.back_alpha);

    /* Alignment */
    const QList<QAbstractButton *> c_pButtonAlignment = m_pButtonGroupAlignment->buttons();

    for(int i = 0; i < c_pButtonAlignment.length(); i++)
    {
        if(c_pButtonAlignment.at(i)->property(TCC_PROP_ALIGNMENT) == a_tccAttr.alignment)
        {
            c_pButtonAlignment.at(i)->setChecked(true);
            break;
        }
    }

    /* Font */
    ui->comboBoxFontName->setCurrentText(Common::fromWCharT(a_tccAttr.font_face_name));
    ui->comboBoxFontNum->clear();
    ui->comboBoxFontNum->setCurrentText(QString::number(a_tccAttr.font_face_id));
    ui->editFontFile->setText(Common::fromWCharT(a_tccAttr.font_file));
    ui->spinBoxFontSize->setValue(a_tccAttr.font_size);

    /* Margins */
    ui->spinBoxOffsetX->setValue(a_tccAttr.x_offset);
    ui->spinBoxOffsetY->setValue(a_tccAttr.y_offset);
    ui->spinBoxSpacing->setValue(a_tccAttr.spacing);
    ui->spinBoxWidth->setValue(a_tccAttr.fx_width);
    ui->spinBoxHeight->setValue(a_tccAttr.fx_height);
    ui->doubleSpinBoxFPS->setValue(a_tccAttr.fx_fps);
}

TCC_Attributes TccDesigner::getTccAttr(void)
{
    TCC_Attributes tccAttr;

    memset(&tccAttr, NULL, sizeof(TCC_Attributes));

    /* Source */
    QJsonArray jsonArray = Common::toJsonArray(ui->listWidgetLine->property(TCC_PY_LIST_JSON_ARRAY_PROP).toString());
    size_t pyFileCount = static_cast<size_t>(jsonArray.count());

    tccAttr.k_timed_ass_file = Common::toWCharT(ui->editSource->text());
    tccAttr.py_file_count = static_cast<int>(pyFileCount);
    tccAttr.beg_line = static_cast<int *>(malloc(pyFileCount * sizeof(int)));
    tccAttr.end_line = static_cast<int *>(malloc(pyFileCount * sizeof(int)));

    tccAttr.py_file = static_cast<wchar_t **>(malloc(pyFileCount * sizeof(wchar_t *)));
    for(int i = 0; i < jsonArray.count(); i++)
    {
        QJsonObject json = jsonArray.at(i).toObject();
        wchar_t *pyFile = Common::toWCharT(json[TCC_JSON_PY_FILE].toString());
        int begLine = json[TCC_JSON_BEG_LINE].toInt();
        int endLine = json[TCC_JSON_END_LINE].toInt();

        tccAttr.py_file[i] = pyFile;
        tccAttr.beg_line[i] = begLine;
        tccAttr.end_line[i] = endLine;
    }

    /* Effect */
    tccAttr.k_mode = ui->comboBoxKaraokeMode->currentIndex() + TCC_INDEX_OFFSET;
    tccAttr.text_layout = ui->comboBoxTextLayout->currentIndex() + TCC_INDEX_OFFSET;
    tccAttr.tcaxpy_init = static_cast<int>(ui->checkBoxUsingTcaxPyInit->isChecked());
    tccAttr.tcaxpy_fin = static_cast<int>(ui->checkBoxUsingTcaxPyFin->isChecked());
    tccAttr.tcaxpy_user = static_cast<int>(ui->checkBoxUsingTcaxPyUser->isChecked());
    tccAttr.inherit_ass_header = static_cast<int>(ui->checkBoxUsingSourceHeader->isChecked());

    /* Color */
    tccAttr.blur = ui->doubleSpinBoxBlur->value();
    tccAttr.space_scale = ui->doubleSpinBoxSpaceScale->value();
    tccAttr.bord = ui->spinBoxBord->value();
    tccAttr.shad = ui->spinBoxShadow->value();

    wcscpy_s(tccAttr.primary_color, Common::toWCharT(ui->editPrimaryColor->text()));
    wcscpy_s(tccAttr.secondary_color, Common::toWCharT(ui->editSecondaryColor->text()));
    wcscpy_s(tccAttr.outline_color, Common::toWCharT(ui->editBordColor->text()));
    wcscpy_s(tccAttr.back_color, Common::toWCharT(ui->editShadowColor->text()));
    tccAttr.back_color[TCC_COLOR_LENGTH - TCC_INDEX_OFFSET] = TCC_COLOR_LAST_CHAR;
    tccAttr.secondary_color[TCC_COLOR_LENGTH - TCC_INDEX_OFFSET] = TCC_COLOR_LAST_CHAR;
    tccAttr.outline_color[TCC_COLOR_LENGTH - TCC_INDEX_OFFSET] = TCC_COLOR_LAST_CHAR;
    tccAttr.back_color[TCC_COLOR_LENGTH - TCC_INDEX_OFFSET] = TCC_COLOR_LAST_CHAR;

    tccAttr.primary_alpha = ui->spinBoxPrimaryColorAlpha->value();
    tccAttr.secondary_alpha = ui->spinBoxSecondaryColorAlpha->value();
    tccAttr.outline_alpha = ui->spinBoxBordColorAlpha->value();
    tccAttr.back_alpha = ui->spinBoxShadowColorAlpha->value();

    /* Alignment */
    tccAttr.alignment = m_pButtonGroupAlignment->checkedButton()->property(TCC_PROP_ALIGNMENT).toInt();

    /* Font */
    //tccAttr.font_face_name = Common::toWCharT(ui->comboBoxFontName->currentText());
    tccAttr.font_face_name = Common::toWCharT(NULLSTR);
    tccAttr.font_face_id = ui->comboBoxFontNum->currentIndex() + TCC_INDEX_OFFSET;
    tccAttr.font_file = Common::toWCharT(ui->editFontFile->text());
    tccAttr.font_size = ui->spinBoxFontSize->value();

    /* Margins */
    tccAttr.x_offset = ui->spinBoxOffsetX->value();
    tccAttr.y_offset = ui->spinBoxOffsetY->value();
    tccAttr.spacing = ui->spinBoxSpacing->value();
    tccAttr.fx_width = ui->spinBoxWidth->value();
    tccAttr.fx_height = ui->spinBoxHeight->value();
    tccAttr.fx_fps = ui->doubleSpinBoxFPS->value();

    return tccAttr;
}

void TccDesigner::on_comboBoxFontName_activated(const QString &a_text)
{
    if(a_text.isEmpty())
    {
        return;
    }

    ui->comboBoxFontNum->clear();
    ui->comboBoxFontNum->addItems(TcaxHelper::fontFace(a_text));
    ui->editFontFile->setText(TcaxHelper::fontFile(a_text));
}

void TccDesigner::on_listWidgetLine_customContextMenuRequested(const QPoint &)
{
    m_pPyListMenu->exec(QCursor::pos());
}

void TccDesigner::setPythonLines(const QJsonArray &a_jsonArray)
{
    ui->listWidgetLine->clear();

    for(int i = 0; i < a_jsonArray.count(); i++)
    {
        QJsonObject json = a_jsonArray.at(i).toObject();

        ui->listWidgetLine->addItem(QString("Py%1: \"%2\" (%3, %4)")
                                    .arg(i + 1)
                                    .arg(json[TCC_JSON_PY_FILE].toString())
                                    .arg(json[TCC_JSON_BEG_LINE].toInt())
                                    .arg(json[TCC_JSON_END_LINE].toInt()));
    }

    ui->listWidgetLine->setProperty(TCC_PY_LIST_JSON_ARRAY_PROP, Common::fromJsonArray(a_jsonArray).simplified());
}

void TccDesigner::lineChanged(void)
{
    QString id = static_cast<QObject *>(sender())->property(ACTION_PROP_ID).toString();
    QJsonArray jsonArray = Common::toJsonArray(ui->listWidgetLine->property(TCC_PY_LIST_JSON_ARRAY_PROP).toString());

    if(id == TCC_PY_LIST_MENU_ADD)
    {
        QJsonObject json;
        QString pyFile = ui->editPython->text();
        int begLine = ui->spinBoxLineStart->value();
        int endLine = ui->spinBoxLineEnd->value();

        if(pyFile.isEmpty())
        {
            QMessageBox::information(this, MSG_INFORMATION, tr("Please input python script for the first time."));
            return;
        }

        json.insert(TCC_JSON_PY_FILE, pyFile);
        json.insert(TCC_JSON_BEG_LINE, begLine);
        json.insert(TCC_JSON_END_LINE, endLine);
        jsonArray.append(json);
    }
    else if(id == TCC_PY_LIST_MENU_DELETE)
    {
        jsonArray.removeAt(ui->listWidgetLine->currentRow());
    }
    else if(id == TCC_PY_LIST_MENU_MOVE_UP)
    {
        int row = ui->listWidgetLine->currentRow();
        QJsonObject json = jsonArray.at(row).toObject();

        if(row > 0)
        {
            jsonArray.removeAt(row);
            jsonArray.insert(row - 1, json);
        }
    }
    else if(id == TCC_PY_LIST_MENU_MOVE_DOWN)
    {
        int row = ui->listWidgetLine->currentRow();
        QJsonObject json = jsonArray.at(row).toObject();

        if( (row >= 0) && (row < jsonArray.count() - 1) )
        {
            jsonArray.removeAt(row);
            jsonArray.insert(row + 1, json);
        }
    }
    setPythonLines(jsonArray);
}

void TccDesigner::on_buttonBrowseSource_clicked()
{
    QString folder = static_cast<MainWindow *>(mainUi)->folder();
    QString filename = QFileDialog::getOpenFileName(this, tr("Open KASS file"), folder, tr("KASS (*.ass *.ssa)"));

    if(!filename.isEmpty())
    {
        ui->editSource->setText(getSrcFilename(filename));
    }
}

void TccDesigner::on_buttonBrowsePython_clicked()
{
    QString folder = static_cast<MainWindow *>(mainUi)->folder();
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Python Script"), folder, tr("Python Script (*.py)"));

    if(!filename.isEmpty())
    {
        ui->editPython->setText(getSrcFilename(filename));
    }
}

QString TccDesigner::getSrcFilename(const QString &a_filename)
{
    QString folder = static_cast<MainWindow *>(mainUi)->folder();
    QDir folderDir(folder);
    QString filenameSrc;

    if(QFileInfo(a_filename).absoluteDir() == folderDir)
    {
        filenameSrc = QFileInfo(a_filename).fileName();
    }
    else
    {
        filenameSrc = QDir::toNativeSeparators(a_filename);
    }
    return filenameSrc;
}

void TccDesigner::selectColor(void)
{
    const static QMap<QPushButton *, QLineEdit *> c_buttonEditorPairList = {
        { ui->buttonPrimaryColor, ui->editPrimaryColor },
        { ui->buttonSecondaryColor, ui->editSecondaryColor },
        { ui->buttonBordColor, ui->editBordColor },
        { ui->buttonShadowColor, ui->editShadowColor },
    };
    QPushButton *p_sender = static_cast<QPushButton *>(sender());
    QLineEdit *p_target = c_buttonEditorPairList.contains(p_sender) ? c_buttonEditorPairList[p_sender] : nullptr;
    QString colorPrevString;
    QColor color;

    if(p_target == nullptr)
    {
        return;
    }

    colorPrevString = p_target->text();

    if(colorPrevString.isEmpty() || colorPrevString.length() < eINDEX_6)
    {
        color = QColorDialog::getColor();
    }
    else
    {
        color = QColorDialog::getColor(DEFAULT_COLOR_INIT + colorPrevString);
    }

    if (!color.isValid())
    {
        return;
    }

    p_target->setText(color.name(QColor::HexRgb).toUpper().remove(DEFAULT_COLOR_INIT));

}
