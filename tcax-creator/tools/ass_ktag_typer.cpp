#include "ass_ktag_typer.h"
#include "ui_ass_ktag_typer.h"

AssKtagTyper::AssKtagTyper(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AssKtagTyper)
{
    ui->setupUi(this);
    this->setup();
}

AssKtagTyper::~AssKtagTyper()
{
    delete m_input_highlighter;
    delete m_output_highlighter;
    delete m_pTextEditMenu;
    delete ui;
}

void AssKtagTyper::setup(void)
{
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    this->setWindowTitle(tr("ASS K-tag Typer (KASS)"));
    this->setFont(DEFAULT_FONT);
    this->setAcceptDrops(true);
    this->initParm();
}

void AssKtagTyper::initParm(void)
{
    m_input_highlighter = new AssSyntaxHighlighter(ui->inputEdit->document());
    m_output_highlighter = new AssSyntaxHighlighter(ui->outputEdit->document());
    this->setHighlight();
    m_clipboard = QApplication::clipboard();

    isTimeLimted = false;
    kCount = eINDEX_0;

    ui->inputEdit->setWordWrapMode(QTextOption::NoWrap);
    ui->outputEdit->setWordWrapMode(QTextOption::NoWrap);
    ui->inputEdit->setAcceptDrops(false);
    ui->outputEdit->setAcceptDrops(false);

    setActions();
}

void AssKtagTyper::setHighlight(void)
{
    this->setEditorHighlightSynth(m_input_highlighter);
    this->setEditorHighlightSynth(m_output_highlighter);
}

void AssKtagTyper::setActions(void)
{
    /* File System Menu */
    USING SCOPE
    {
        QWidget *action_parent = this;
        QList<QPair<QString, QAction *>> action_list = {
            ACTION_NEW(ACTION_KASS_MENU_MAKE, ":/buttons/wand.png", action_parent),
            ACTION_NEW(ACTION_KASS_MENU_OPEN, ":/buttons/folder_page_white.png", action_parent),
            ACTION_NONE,
            ACTION_NEW(ACTION_KASS_MENU_COPY, ":/buttons/page_paste.png", action_parent),
            ACTION_NEW(ACTION_KASS_MENU_SAVE_HERE, ":/buttons/disk.png", action_parent),
            ACTION_NEW(ACTION_KASS_MENU_SAVE_AS, ":/buttons/disk_multiple.png", action_parent),
            ACTION_NEW(ACTION_KASS_MENU_CLEAR, ":/buttons/erase.png", action_parent),
            ACTION_NONE,
            ACTION_NEW(ACTION_KASS_MENU_ENABLE_SCROLL_SYNC, ":/buttons/synchronous_scrolling.png", action_parent),
            ACTION_NEW(ACTION_KASS_MENU_ENABLE_WRAP_LINE, ":/buttons/text_wrapping.png", action_parent),
        };

        m_pTextEditMenu = new QMenu(action_parent);

        for(int i = 0; i < action_list.length(); i++)
        {
            QString key = action_list.at(i).first;
            QAction *value = action_list.at(i).second;

            if( (key == NULLSTR) || (value == nullptr) )
            {
                m_pTextEditMenu->addSeparator();
            }
            else
            {
                m_pTextEditMenu->addAction(value);
                value->setProperty(ACTION_PROP_ID, key);
                value->setCheckable( (key == ACTION_KASS_MENU_ENABLE_WRAP_LINE || key == ACTION_KASS_MENU_ENABLE_SCROLL_SYNC) ? true : false);
                value->setChecked( (key == ACTION_KASS_MENU_ENABLE_SCROLL_SYNC) ? true : false);
                value->connect(value, SIGNAL(triggered(bool)), this, SLOT(slotActions(bool)));
            }
        }

        QList<QPair<QString, QAbstractButton *>> button_list = {
            { ACTION_KASS_MENU_OPEN, ui->buttonOpen },
            { ACTION_KASS_MENU_SAVE_HERE, ui->buttonSave },
            { ACTION_KASS_MENU_SAVE_AS, ui->buttonSaveAs },
        };

        for(int i = 0; i < button_list.length(); i++)
        {
            QString key = button_list.at(i).first;
            QAbstractButton *value = button_list.at(i).second;

            value->setProperty(ACTION_PROP_ID, key);
            value->connect(value, SIGNAL(clicked(bool)), this, SLOT(slotActions(bool)));
        }

        connect(ui->inputEdit->verticalScrollBar(), &QAbstractSlider::valueChanged, this, &AssKtagTyper::slotTextEditSliderVertical);
        connect(ui->outputEdit->verticalScrollBar(), &QAbstractSlider::valueChanged, this, &AssKtagTyper::slotTextEditSliderVertical);
        connect(ui->inputEdit->horizontalScrollBar(), &QAbstractSlider::valueChanged, this, &AssKtagTyper::slotTextEditSliderHorizontal);
        connect(ui->outputEdit->horizontalScrollBar(), &QAbstractSlider::valueChanged, this, &AssKtagTyper::slotTextEditSliderHorizontal);
        connect(ui->inputEdit, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(slotTextEditMenuRequested()));
        connect(ui->outputEdit, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(slotTextEditMenuRequested()));
    }
}

void AssKtagTyper::setEditorHighlightSynth(AssSyntaxHighlighter *highlighter)
{
    QStringList keywords;

    keywords << QString(ASS_TAG_SOFT_EOL).left(eINDEX_1) + QString(ASS_TAG_SOFT_EOL)
             << QString(ASS_TAG_HARD_EOL).left(eINDEX_1) + QString(ASS_TAG_HARD_EOL)
             << QString(ASS_TAG_BLANK).left(eINDEX_1) + QString(ASS_TAG_BLANK);
    highlighter->addSyntax(QString(ASS_TAG_BRACE_REGEXP), highlighter->keyword_format);
    highlighter->addSyntax(QString(ASS_TAG_COMMENT_SCRIPT_REGEXP), highlighter->comment_line_format);
    highlighter->addSyntax(QString(ASS_TAG_COMMENT_TEXT_REGEXP), highlighter->comment_line_format);
    highlighter->addSyntax(QString(ASS_TAG_SECTION_REGEXP), highlighter->section_line_format);
    highlighter->addSyntax(QString(ASS_TAG_FORMAT_REGEXP), highlighter->format_line_format);
    highlighter->addSyntax(QString(ASS_TAG_STYLE_REGEXP), highlighter->style_line_format);
    highlighter->addSyntaxKeywords(keywords, highlighter->keyword_format);
}

void AssKtagTyper::slotHighlight(void)
{
    if(ui->inputEdit->toPlainText().isEmpty() && ui->inputEdit->toPlainText().isEmpty())
    {
        return;
    }
    if(!m_input_highlighter->isHasRules() || !m_output_highlighter->isHasRules())
    {
        m_input_highlighter->clearSyntax();
        m_output_highlighter->clearSyntax();
    }
    else
    {
        this->setHighlight();
    }

    /*Save scroll bar value*/
    int input_ver_value = ui->inputEdit->verticalScrollBar()->value();
    int output_ver_value = ui->outputEdit->verticalScrollBar()->value();
    int input_hor_value = ui->inputEdit->horizontalScrollBar()->value();
    int output_hor_value = ui->outputEdit->horizontalScrollBar()->value();

    /*Set text*/
    ui->inputEdit->document()->setPlainText(ui->inputEdit->toPlainText());
    ui->outputEdit->document()->setPlainText(ui->outputEdit->toPlainText());

    /*Reload scroll bar value*/
    ui->inputEdit->verticalScrollBar()->setValue(input_ver_value);
    ui->outputEdit->verticalScrollBar()->setValue(output_ver_value);
    ui->inputEdit->horizontalScrollBar()->setValue(input_hor_value);
    ui->outputEdit->horizontalScrollBar()->setValue(output_hor_value);
}


void AssKtagTyper::reload(const QString &path)
{
    QFile file(path);
    QStringList lines;
    QTextStream in(&file);

    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug()<<"Can't open the file!"<<endl;
    }
    while(!in.atEnd())
    {
        QString line = in.readLine();

        lines.append(line);
    }
    ui->inputEdit->document()->setPlainText(lines.join(QT_NOR_EOL));
    convert();
}

void AssKtagTyper::pasteText(void)
{
    ui->inputEdit->document()->setPlainText(m_clipboard->text());
}

void AssKtagTyper::saveDiskFile(void)
{
    QString path = QString("%1_k.%2").arg(Common::delFileExt(input_path)).arg(Common::getFileExt(input_path));

    if(QFile::exists(path))
    {
        int result = QMessageBox::warning(this, MSG_WARNING, tr("Overwrite existing file?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

        if(result == QMessageBox::No)
        {
            return;
        }
    }
    save(path);
}

void AssKtagTyper::saveAsDiskFile(void)
{
    QString path = QFileDialog::getSaveFileName(this, tr("Open File"), "/", tr("Ass Files (*.ass *.ssa *.txt)"));

    save(path);
}

void AssKtagTyper::save(const QString &a_path)
{
    QString text = ui->outputEdit->toPlainText();

    if(text.isEmpty())
    {
        QMessageBox::information(this, MSG_INFORMATION, tr("Empty output."));
        return;
    }
    if(a_path.isEmpty())
    {
        return;
    }

    if(!Common::setFileText(a_path, Common::toDosEol(text), true))
    {
        QMessageBox::warning(this, MSG_WARNING, tr("Can't open output file."), QMessageBox::Yes);
    }
}

void AssKtagTyper::copy()
{
    QString text = ui->outputEdit->toPlainText();

    if(text.isEmpty())
    {
        return;
    }
    m_clipboard->setText(text);
}

void AssKtagTyper::closeEvent(QCloseEvent *event)
{
    event->accept();
}

void AssKtagTyper::convert()
{
    QString ass_text = ui->inputEdit->toPlainText();

    if(ass_text.isEmpty())
    {
        return;
    }

    QStringList ass_text_k_list;
    QStringList ass_text_list = ass_text.split(QT_NOR_EOL);
    for(QString ass_text : ass_text_list)
    {
        if(ass_text.startsWith(ASS_TAG_EVENT_FORMAT_DIALOGUE) || ass_text.startsWith(ASS_TAG_EVENT_FORMAT_COMMENT))
        {
            if(ui->stripTagCheckBox->isChecked())
            {
                ass_text = stripTag(ass_text);
            }
            QString ass_text_k = callAssKtagTyper(ass_text);
            if(!ass_text_k.isEmpty())
            {
                ass_text_k_list.append(ass_text_k);
            }
        }
        else
        {
            ass_text_k_list.append(ass_text);
        }
    }
    /*Save scroll bar value*/
    int input_ver_value = ui->inputEdit->verticalScrollBar()->value();
    int output_ver_value = ui->outputEdit->verticalScrollBar()->value();
    int input_hor_value = ui->inputEdit->horizontalScrollBar()->value();
    int output_hor_value = ui->outputEdit->horizontalScrollBar()->value();

    /*Set text*/
    ui->outputEdit->document()->setPlainText(ass_text_k_list.join(QT_NOR_EOL));

    /*Reload scroll bar value*/
    ui->inputEdit->verticalScrollBar()->setValue(input_ver_value);
    ui->outputEdit->verticalScrollBar()->setValue(output_ver_value);
    ui->inputEdit->horizontalScrollBar()->setValue(input_hor_value);
    ui->outputEdit->horizontalScrollBar()->setValue(output_hor_value);
}

float AssKtagTyper::calcTextLength(QString text)
{
    QString text_prev = text;
    float length = text.length();
    if(    text.indexOf(ASS_TAG_BLANK)    == eINDEX_NONE
        && text.indexOf(ASS_TAG_BLANK2)   == eINDEX_NONE
        && text.indexOf(ASS_TAG_HARD_EOL) == eINDEX_NONE
        && text.indexOf(ASS_TAG_SOFT_EOL) == eINDEX_NONE
        && text.indexOf(QT_BLANK)         == eINDEX_NONE )
    {
        /* Calc all. */
        PASS;
    }
    else
    {
        /* Calc whitout blank. */
        QStringList skip_texts;
        skip_texts << ASS_TAG_BLANK << ASS_TAG_BLANK2 << ASS_TAG_HARD_EOL << ASS_TAG_SOFT_EOL;
        for(QString skip_text : skip_texts)
        {
            text = text.remove(skip_text);
        }
        length = text.length();
    }
    int syl_num = ui->sylComboBox->currentIndex();
    if(syl_num != eINDEX_0)
    {
        syl_num++;
        length /= static_cast<float>(syl_num);
        if(length < eINDEX_1)
        {
            length = eINDEX_1;
        }
    }
    if(static_cast<int>(length) != text_prev.length())
    {
        //PASS;
    }
    return length;
}

QString AssKtagTyper::getAssText(QStringList events)
{
    QString text = QT_EMPTY;
    if(events.length() < static_cast<int>(AssEvents::MaxEvent))
    {
        PASS;
    }
    else if(events.length() == static_cast<int>(AssEvents::MaxEvent))
    {
        text = events.at(static_cast<int>(AssEvents::Text));
    }
    else if(events.length() > static_cast<int>(AssEvents::MaxEvent))
    {
        QStringList texts;

        for(int i = static_cast<int>(AssEvents::Text); i < events.length(); i++)
        {
            texts.append(events.at(i));
        }
        text = texts.join(ASS_TAG_COMMA);
    }
    return text;
}

QString AssKtagTyper::callAssKtagTyper(QString rv_text)
{
    QString text_k = QString(QT_EMPTY);
    QStringList events = rv_text.split(ASS_TAG_COMMA);

    if(events.length() < static_cast<int>(AssEvents::MaxEvent))
    {
        return text_k;
    }

    QString start = events.at(static_cast<int>(AssEvents::Start));
    QString end = events.at(static_cast<int>(AssEvents::End));
    QString texts = getAssText(events);
    float text_length = calcTextLength(texts);
    QString k_tag = this->calcAssKtagTyperValue(start, end, text_length);

    /* Foreach without text. */
    int at_count = eINDEX_0;

    for(QString event : events)
    {
        if(at_count >= (static_cast<int>(AssEvents::MaxEvent) - eINDEX_1))
        {
            text_k += QString(ASS_TAG_COMMA);
            break;
        }
        if(at_count == eINDEX_0)
        {
            text_k = event;
        }
        else
        {
            text_k = QString(QT_STR_ARG_3).arg(text_k).arg(ASS_TAG_COMMA).arg(event);
        }
        at_count++;
    }
    /* Call K Value */
    at_count = eINDEX_0;

    int at_skip_count = eINDEX_0;
    int at_syl_num = ui->sylComboBox->currentIndex();
    int at_syl_count = eINDEX_0;
    bool is_blank_skip = false;
    bool is_syl_skip = false;
    bool is_syl_first_not_skip_ok = false;
    int text_length_count = eINDEX_0;

    for(QString text : texts)
    {
        if(ui->skipBlankCheckBox->isChecked())
        {
            if(text == ASS_TAG_SKIP_FLAG)
            {
                if(at_count + eINDEX_1 <= texts.length() - eINDEX_1 )
                {
                    /* Index not overflow */
                    QString at_check = QString(QT_STR_ARG_2).arg(ASS_TAG_SKIP_FLAG).arg(texts.at(at_count + eINDEX_1));
                    QStringList skip_texts;

                    skip_texts << ASS_TAG_BLANK << ASS_TAG_BLANK2 << ASS_TAG_HARD_EOL << ASS_TAG_SOFT_EOL;
                    for(QString skip_text : skip_texts)
                    {
                        if(at_check == skip_text)
                        {
                            is_blank_skip = true;
                            break;
                        }
                    }
                }
            }
        }

        if(at_syl_num != eINDEX_0)
        {
            if(is_syl_first_not_skip_ok)
            {
                is_syl_skip = true;
            }
            else if(!is_blank_skip)
            {
                is_syl_first_not_skip_ok = true;
            }
        }

        if(is_syl_skip)
        {
            at_syl_count++;
            if(at_syl_count >= at_syl_num + eINDEX_1/*Cause of Not Skip*/)
            {
                is_syl_skip = false;
                at_syl_count = eINDEX_0;
            }
        }
        if(is_blank_skip || is_syl_skip)
        {
            /* Tag Skip */
            text_k = QString(QT_STR_ARG_2).arg(text_k).arg(text);
        }
        else
        {
            /* Tag Addition */
            if(isTimeLimted)
            {
                /* The k_tag is {\\k1} */
                text_length_count++;
                if(text_length_count <=  kCount)
                {
                    /* Add tag {\\k1} */
                    PASS;
                }
                else
                {
                    /* Add tag {\\k0} */
                    k_tag = QString(QT_STR_TAG).arg(ASS_TAG_KARAOKE_K_LOWER).arg(QString::number(eINDEX_0));
                }
            }
            text_k = QString(QT_STR_ARG_3).arg(text_k).arg(k_tag).arg(text);
        }

        if(is_blank_skip)
        {
            at_skip_count++;
            if(at_skip_count >= ASS_TAG_SKIP_NUM)
            {
                is_blank_skip = false;
                at_skip_count = eINDEX_0;
            }
        }
        at_count++;
    }
    return text_k;
}


QString AssKtagTyper::calcAssKtagTyperValue(QString start, QString end, float length)
{
    isTimeLimted = false;
    kCount = eINDEX_0;
    QTime startTime = QTime::fromString(start, ASS_TAG_TIMECODE_FORMAT);
    QTime endTime = QTime::fromString(end, ASS_TAG_TIMECODE_FORMAT);
    QString k_tag = QT_EMPTY;
    int k_value = eINDEX_0;

    if(static_cast<int>(length) == eINDEX_0)
    {
        return QT_EMPTY;
    }
    if(!ui->otherTagCheckBox->isChecked())
    {
        /* K(lower) tag. */
        switch(static_cast<CallK>(ui->karaokeComboBox->currentIndex()))
        {
        case CallK::Average:
            do
            {
                int elapsed_msecs = startTime.msecsTo(endTime);
                k_value = static_cast<int>((static_cast<float>(elapsed_msecs) / static_cast<float>(length)) / 10);
                if(k_value == eINDEX_0)
                {
                    /*Time is limited*/
                    isTimeLimted = true;
                    kCount = static_cast<int>(elapsed_msecs / 10);
                    k_value = eINDEX_1;
                }
            }while(false);
            break;
        case CallK::Fixed:
            k_value = ui->fixedKaraokeSpinBox->value();
            break;
        default:
            break;
        }
        k_tag = QString(QT_STR_TAG).arg(ASS_TAG_KARAOKE_K_LOWER).arg(QString::number(k_value));
    }
    else
    {
        /* Other tag. */
        k_tag = ui->otherTagLineEdit->text();
        k_tag = QString(QT_STR_TAG_OTR).arg(k_tag);
    }
    return k_tag;
}

QString AssKtagTyper::stripTag(QString text)
{
    return text.replace(QRegExp(ASS_TAG_BRACE_REGEXP), QT_EMPTY);
}

void AssKtagTyper::on_karaokeComboBox_activated(int index)
{
    switch(static_cast<CallK>(index))
    {
    case CallK::Average:
        ui->fixedKaraokeSpinBox->setEnabled(false);
        break;
    case CallK::Fixed:
        ui->fixedKaraokeSpinBox->setEnabled(true);
        break;
    case CallK::Other:
    default:
        break;
    }
}

void AssKtagTyper::on_otherTagCheckBox_stateChanged(int arg1)
{
    if(arg1)
    {
        ui->typeLabel->setEnabled(false);
        ui->karaokeComboBox->setEnabled(false);
        ui->fixedKaraokeSpinBox->setEnabled(false);
        ui->otherTagLineEdit->setEnabled(true);
    }
    else
    {
        ui->typeLabel->setEnabled(true);
        ui->karaokeComboBox->setEnabled(true);
        emit ui->karaokeComboBox->activated(ui->karaokeComboBox->currentIndex());
        ui->otherTagLineEdit->setEnabled(false);
    }
}

void AssKtagTyper::dragEnterEvent(QDragEnterEvent* event)
{
    if(event->mimeData()->hasText())
    {
        event->acceptProposedAction();
    }
}

void AssKtagTyper::dropEvent(QDropEvent* event)
{
    const QMimeData* mime = event->mimeData();

    if(mime->hasUrls())
    {
        foreach(QUrl url, event->mimeData()->urls())
        {
            QFileInfo file(url.toLocalFile());
            QString filename = QDir::toNativeSeparators(file.canonicalFilePath());
            QString ext = QFileInfo(filename).suffix();

            if(!QFileInfo(filename).isFile())
            {
                continue;
            }
            if(ext == ASS_EXT_TYPE_A || ext == ASS_EXT_TYPE_B || ext == ASS_EXT_TYPE_C)
            {
                input_path = filename;
                reload(filename);
            }
            break;
        }
    }
    event->acceptProposedAction();
}

void AssKtagTyper::dragMoveEvent(QDragMoveEvent *event)
{
    event->pos();
    PASS;
}

void AssKtagTyper::slotTextEditSliderVertical(int value)
{
    ui->inputEdit->verticalScrollBar()->setValue(value);
    ui->outputEdit->verticalScrollBar()->setValue(value);
}

void AssKtagTyper::slotTextEditSliderHorizontal(int value)
{
    ui->inputEdit->horizontalScrollBar()->setValue(value);
    ui->outputEdit->horizontalScrollBar()->setValue(value);
}

void AssKtagTyper::slotTextEditMenuRequested(void)
{
    m_pTextEditMenu->exec(QCursor::pos());
}

void AssKtagTyper::slotActions(bool a_checked)
{
    QString id = sender()->property(ACTION_PROP_ID).toString();

    if(id == ACTION_KASS_MENU_MAKE)
    {
        convert();
    }
    else if(id == ACTION_KASS_MENU_OPEN)
    {
        QString path = QFileDialog::getOpenFileName(this, tr("Open File"), "/", tr("Ass Files (*.ass *.ssa *.txt)"));

        if(path.isEmpty())
        {
            return;
        }
        input_path = path;
        reload(path);
    }
    else if(id == ACTION_KASS_MENU_COPY)
    {
        QClipboard *board = QApplication::clipboard();

        if(!ui->outputEdit->toPlainText().isEmpty())
        {
            board->setText(ui->outputEdit->toPlainText());
        }
    }
    else if(id == ACTION_KASS_MENU_SAVE_HERE)
    {
        saveDiskFile();
    }
    else if(id == ACTION_KASS_MENU_SAVE_AS)
    {
        saveAsDiskFile();
    }
    else if(id == ACTION_KASS_MENU_CLEAR)
    {
        ui->inputEdit->clear();
        ui->outputEdit->clear();
    }
    else if(id == ACTION_KASS_MENU_ENABLE_SCROLL_SYNC)
    {
        if(a_checked)
        {
            connect(ui->inputEdit->verticalScrollBar(), &QAbstractSlider::valueChanged, this, &AssKtagTyper::slotTextEditSliderVertical);
            connect(ui->outputEdit->verticalScrollBar(), &QAbstractSlider::valueChanged, this, &AssKtagTyper::slotTextEditSliderVertical);
            connect(ui->inputEdit->horizontalScrollBar(), &QAbstractSlider::valueChanged, this, &AssKtagTyper::slotTextEditSliderHorizontal);
            connect(ui->outputEdit->horizontalScrollBar(), &QAbstractSlider::valueChanged, this, &AssKtagTyper::slotTextEditSliderHorizontal);
        }
        else
        {
            disconnect(ui->inputEdit->verticalScrollBar(), &QAbstractSlider::valueChanged, this, &AssKtagTyper::slotTextEditSliderVertical);
            disconnect(ui->outputEdit->verticalScrollBar(), &QAbstractSlider::valueChanged, this, &AssKtagTyper::slotTextEditSliderVertical);
            disconnect(ui->inputEdit->horizontalScrollBar(), &QAbstractSlider::valueChanged, this, &AssKtagTyper::slotTextEditSliderHorizontal);
            disconnect(ui->outputEdit->horizontalScrollBar(), &QAbstractSlider::valueChanged, this, &AssKtagTyper::slotTextEditSliderHorizontal);
        }
    }
    else if(id == ACTION_KASS_MENU_ENABLE_WRAP_LINE)
    {
        if(a_checked)
        {
            ui->inputEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);
            ui->outputEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);
            ui->inputEdit->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
            ui->outputEdit->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
            qDebug() << a_checked;
        }
        else
        {
            ui->inputEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
            ui->outputEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
            ui->inputEdit->setWordWrapMode(QTextOption::NoWrap);
            ui->outputEdit->setWordWrapMode(QTextOption::NoWrap);
            qDebug() << a_checked;
        }
    }
}

