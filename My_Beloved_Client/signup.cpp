#include "signup.h"
#include "ui_signup.h"
#include "client.h"
#include <QMessageBox>

Signup::Signup(Client *client, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::Signup),
    client(client)
{
    ui->setupUi(this);

    //یو ای طوری مدیریت شود که تا وقتی همه تکست ها حداقل یک کاراکتر ندارند دکمه ساین این فعال نباشد مشابه این کار در لاگین انجام شده است
    //وقتی کاربر دکمه ساین این را وارد کرد با توجه به یو ای چک شود که اطلاعات درست هستند یا خیر مثلا معتبر بودن رمز
    // مثلا معتبر بودن ایمیل و شماره و ...
    //همچنین متود بررسی ولید بودن ادرس  باید چک کند که فرمت ادرس به صورت 000.000.000.000هست یا نه
    // وقتی همه اطلاعات اوکی بود متود زیر فراخوانی شود
    // client.WriteToServer(QString data)
    // به فرمت زیر
    // "S[FirstName][LastName][Phone][Email][Username][password]
}


Signup::~Signup()
{
    delete ui;
}





