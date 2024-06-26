#include "widget.h"
#include "./ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    
    ui->assertDaySpinBox->setRange(1, 31);
    ui->assertMonthSpinBox->setRange(1, 12);
    ui->assertYearSpinBox->setRange(1, 9999);
    turnAssert(0);
    setButtons(1);
    ui->saveFileButton->setDisabled(1);



    connect(ui->chooseFile, &QPushButton::clicked,this, [this](){

        QFileDialog newFile;

        openFile = new QFile (QFileDialog::getOpenFileName(this, "Выбор файла", "/home/yauheni", "text files (*.txt)"));
        openFile->open(QIODevice::ReadOnly| QIODevice::Text);
        openFile->seek(0);
        data.clear();

        QString readedText = "";
        while (openFile->pos()!=openFile->size()) {
            QString readedContent = openFile->readLine(2);
            if(readedContent ==' '){
                qDebug()<<readedText;
                if(Date(readedText).isCorrectDate())data.push_back(Date (readedText));
                readedText = "";
            }
            else{
                readedText += readedContent;
            }
        }
        ui->saveFileButton->setDisabled(0);
        showDate();
        openFile->close();
    });

    connect(ui->weekNumberButton, &QPushButton::clicked, this, [this](){
        ui->weekNumberLabel->setNum(data[ui->listWidget->currentRow()].weekNumber());
    });

    connect(ui->addDateButton, &QPushButton::clicked,this, [this](){
        turnAssert(1);
        currentAction = 1;
        ui->assertLabel->setText("Введите новую дату");
    });

    connect(ui->birthdayButton, &QPushButton::clicked,this, [this](){
        turnAssert(1);
        currentAction = 2;
        ui->assertLabel->setText("Введите дату своего рождения");
    });

    connect(ui->editDateButton, &QPushButton::clicked,this, [this](){
        turnAssert(1);
        currentAction = 3;
        ui->assertLabel->setText("Введите измененную дату");
    });

    connect(ui->durationButton, &QPushButton::clicked,this, [this](){
        turnAssert(1);
        currentAction = 4;
        ui->assertLabel->setText("Введите дату");
    });

    connect(ui->removeDateButton, &QPushButton::clicked,this, [this](){
        data.remove(ui->listWidget->currentRow());
        showDate();
        setButtons(1);
    });

    connect(ui->saveFileButton, &QPushButton::clicked,this, [this](){
        openFile->open(QIODevice::WriteOnly| QIODevice::Text| QIODevice::Truncate);
        openFile->seek(0);
        for(int i = 0; i< data.size(); ++i){
            openFile->write(data[i].getDate().toUtf8() + ' ');
        }
        openFile->close();
    });

    connect(ui->assertConfirmButton, &QPushButton::clicked,this, [this](){
        switch (currentAction) {
        case 1:{
            Date newDate(ui->assertDaySpinBox->value(), ui->assertMonthSpinBox->value(), ui->assertYearSpinBox->value());
            if(newDate.isCorrectDate()){
                data.push_back(newDate);
                showDate();
                turnAssert(0);
            }else{
                ui->assertLabel->setText("Введены некоректные данные");
            }
            break;
        }
        case 2:{
            Date newDate(ui->assertDaySpinBox->value(), ui->assertMonthSpinBox->value(), ui->assertYearSpinBox->value());
            if(newDate.isCorrectDate()&&newDate.canBeBirtday()){
                ui->birthdayLabel->setText(toString(data[ui->listWidget->currentRow()].daysTillYourBirthday(newDate)));
                turnAssert(0);
            }else{
                ui->assertLabel->setText("Введены некоректные данные");
            }
            break;
        }
        case 3:{
            Date newDate(ui->assertDaySpinBox->value(), ui->assertMonthSpinBox->value(), ui->assertYearSpinBox->value());
            if(newDate.isCorrectDate()){
                data[ui->listWidget->currentRow()] = newDate;
                showDate();
                turnAssert(0);
            }else{
                ui->assertLabel->setText("Введены некоректные данные");
            }
            break;
        }
        case 4:{
            Date newDate(ui->assertDaySpinBox->value(), ui->assertMonthSpinBox->value(), ui->assertYearSpinBox->value());
            if(newDate.isCorrectDate()){
                ui->durationLabel->setText(toString(data[ui->listWidget->currentRow()].duration(newDate)));
                turnAssert(0);
            }else{
                ui->assertLabel->setText("Введены некоректные данные");
            }
            break;
        }

        default:
            break;
        }
    });

    connect(ui->listWidget, &QListWidget::itemSelectionChanged,this, [this](){
        setButtons(0);
    });
}

void Widget::dafaultRangeButtons(){
    ui->assertDaySpinBox->setRange(1, 31);
    ui->assertMonthSpinBox->setRange(1, 12);
    ui->assertYearSpinBox->setRange(1, 9999);
}

void Widget::birthdayRangeButtons(){
    ui->assertDaySpinBox->setRange(1, 31);
    ui->assertMonthSpinBox->setRange(1, 12);
    ui->assertYearSpinBox->setRange(1, 9999);
}

void Widget::turnAssert(bool signal){
    ui->assertConfirmButton->setVisible(signal);
    ui->assertDaySpinBox->setVisible(signal);
    ui->assertMonthSpinBox->setVisible(signal);
    ui->assertYearSpinBox->setVisible(signal);
    ui->assertDayLabel->setVisible(signal);
    ui->assertMonthLabel->setVisible(signal);
    ui->assertYearLabel->setVisible(signal);
    ui->assertLabel->setVisible(signal);

}

void Widget::showDate(){
    ui->listWidget->clear();
    for(int i=0; i<data.size(); ++i){
        QString info = "";

        info += data[i].getDate() + "\t  ";
        info += data[i].nextDay().getDate() + "           ";
        info += data[i].previousDay().getDate()+ "\t";
        info += (i!=data.size()-1?toString(data[i].duration(data[i+1])): "  ———")+ "\t\t   ";
        info += (data[i].isLeapYear() ? "Да" : "Нет");

        ui->listWidget->addItem(info);
    }
}

void Widget::setButtons(bool signal){
    ui->removeDateButton->setDisabled(signal);
    ui->birthdayButton->setDisabled(signal);
    ui->durationButton->setDisabled(signal);
    ui->editDateButton->setDisabled(signal);
    ui->weekNumberButton->setDisabled(signal);
}

QString Widget::toString(int num){
    QString str = "";
    int step = num;
    while(step!=0){
        str.append(QChar('0' + step%10));
        step /= 10;
    }
    QString res = "";
    for (int i = str.length()-1; i >= 0; --i) {
        res.append(str[i]);
    }
    if(res == "")return "0" ;
    return res;
}

Widget::~Widget()
{
    delete openFile;
    delete ui;
}
