#include "changepassworddialog.h"
#include "ui_changepassworddialog.h"

#include <security/pam_appl.h>

#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>

#include <QDebug>

int ChangePasswordDialog::conversation(int num_msg,
                                       const struct pam_message **msg,
                                       struct pam_response **resp,
                                       void *appdata_ptr)
{
    *resp = 0;
    ChangePasswordDialog *dialog = 0;
    if (!(num_msg && (dialog = (ChangePasswordDialog *) appdata_ptr) && ! dialog->_userAbort))
        return PAM_CONV_ERR;

    int answerSize = num_msg * sizeof(struct pam_response);
    struct pam_response *answer = 0;

    if (!(answer = (struct pam_response *) malloc(answerSize)))
        return PAM_BUF_ERR;

    memset(answer, 0, answerSize);

    bool labelAdded = false;
    for (int i = 0; i < num_msg; i++)
    {
        QString message = QString::fromLocal8Bit(msg[i]->msg);
        switch (msg[i]->msg_style)
        {
            case PAM_PROMPT_ECHO_OFF:
            {
                labelAdded = true;
                dialog->addLabel(message, true);
            }
            break;
            case PAM_PROMPT_ECHO_ON:
            {
                labelAdded = true;
                dialog->addLabel(message, false);
            }
            break;
            case PAM_ERROR_MSG:
                dialog->addWarning(message);
            break;
            case PAM_TEXT_INFO :
                dialog->addMessage(message);
            break;
            default:
            {
                free(answer);
                return PAM_CONV_ERR;
            }
        }
    }

    if (!labelAdded)
    {
        *resp = answer;
        return PAM_SUCCESS;
    }

    dialog->showWarnings();
    dialog->showMessages();

    if (!(dialog->exec() == QDialog::Accepted))
    {
        dialog->_userAbort = true;
        free(answer);
        return PAM_CONV_ERR;
    }

    for (int i = 0, j = 0; i < num_msg; i++)
    {
        switch (msg[i]->msg_style)
        {
            case PAM_PROMPT_ECHO_OFF:
            case PAM_PROMPT_ECHO_ON:
            {
                char *str = strdup(qPrintable(dialog->values().at(j)));
                j++;
                if (!str)
                {
                    for (int k = 0; k < i; k++)
                        free(answer[k].resp);
                    free(answer);
                    return PAM_BUF_ERR;
                }
                answer[i].resp = str;
            }
            break;
            default :
                break;
        }
    }

    *resp =  answer;
    return PAM_SUCCESS;
}

ChangePasswordDialog::ChangePasswordDialog(QWidget *parent)
: QDialog(parent)
, _ui(new Ui::ChangePasswordDialog)
, _userAbort(false)
{
    _ui->setupUi(this);
}

ChangePasswordDialog::~ChangePasswordDialog()
{
    delete _ui;
}

const QString &ChangePasswordDialog::user() const
{
    return _user;
}

void ChangePasswordDialog::setUser(const QString &user)
{
    if (_user == user)
        return;

    _user = user;
    setWindowTitle(tr("Change password for user \"%1\"").arg(_user));
}

void ChangePasswordDialog::addWarning(const QString &warning)
{
    _warnings.append(warning);
}

void ChangePasswordDialog::addMessage(const QString &message)
{
    _messages.append(message);
}

void ChangePasswordDialog::addLabel(const QString &textLabel, bool isShadow)
{
    QLabel *label = new QLabel();;
    QLineEdit *lineEdit = new QLineEdit();

    label->setText(textLabel);
    lineEdit->setEchoMode(isShadow ? QLineEdit::Password : QLineEdit::Normal);

    _ui->_verticalLayout->addWidget(label);
    _ui->_verticalLayout->addWidget(lineEdit);
}

const QList<QString> &ChangePasswordDialog::values() const
{
    return _values;
}

bool ChangePasswordDialog::changePassword()
{
    if (_user.isEmpty())
        return false;

    _userAbort = false;

    pam_handle_t *pamh = NULL;

    pam_conv pam_conversation;
    pam_conversation.conv = ChangePasswordDialog::conversation;
    pam_conversation.appdata_ptr = (void *) this;

    int result = 0;

    result = pam_start("password", qPrintable(_user), &pam_conversation, &pamh);

    if (result != PAM_SUCCESS)
    {
        QMessageBox::critical(parentWidget(), tr("PAM transaction"), tr("PAM transaction not started.\nError code: %1").arg(result));
        return false;
    }

    bool isChanged = ((result = pam_chauthtok(pamh, 0)) == PAM_SUCCESS);

    showWarnings();
    showMessages();

    if ((result =  pam_end(pamh, result)) != PAM_SUCCESS)
        QMessageBox::critical(parentWidget(), tr("PAM transaction"), tr("PAM transaction not finished.\nError code: %1").arg(result));

    return isChanged;
}

void ChangePasswordDialog::reset()
{
    int size = _ui->_verticalLayout->count();
    for (int i = 0; i < size; i++)
    {
        QLayoutItem *layoutItem = _ui->_verticalLayout->itemAt(0);
        QWidget *widget = layoutItem->widget();
        _ui->_verticalLayout->removeItem(layoutItem);
        if (widget)
            delete widget;
    }
}

void ChangePasswordDialog::showMessages()
{
    if (_messages.size())
    {
        QMessageBox::information(parentWidget(), windowTitle(), _messages.join(QStringLiteral("\n")));
        _messages.clear();
    }
}

void ChangePasswordDialog::showWarnings()
{
    if (_warnings.size())
    {
        QMessageBox::warning(parentWidget(), windowTitle(), _warnings.join(QStringLiteral("\n")));
        _warnings.clear();
    }
}

void ChangePasswordDialog::showEvent(QShowEvent *event)
{
    if (!_ui->_verticalLayout->count())
        return;
    _ui->_verticalLayout->addStretch(1);
    QDialog::showEvent(event);
    _ui->_verticalLayout->itemAt(1)->widget()->setFocus();

}

void ChangePasswordDialog::accept()
{
    _values.clear();

    int size = _ui->_verticalLayout->count();
    for (int i = 0; (i + 1) < size; i += 2)
    {
        QLineEdit *lineEdit = qobject_cast<QLineEdit *>(_ui->_verticalLayout->itemAt(i + 1)->widget());
        if (lineEdit)
            _values.append( lineEdit->text() );
    }
    reset();
    QDialog::accept();
}

void ChangePasswordDialog::reject()
{
    reset();
    QDialog::reject();
}
