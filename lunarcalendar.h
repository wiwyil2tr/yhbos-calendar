#ifndef LUNARCALENDAR_H
#define LUNARCALENDAR_H

#include <QString>
#include <QDate>

class LunarCalendarPrivate;

class LunarCalendar
{
public:
    LunarCalendar();
    ~LunarCalendar();

    void setSolarDate(int year, int month, int day);

    QString lunarYear() const;
    QString lunarMonth() const;
    QString lunarDay() const;
    QString ganzhiYear() const;
    QString ganzhiMonth() const;
    QString ganzhiDay() const;
    QString zodiac() const;
    QString holiday() const;
    QString constellation() const;
    QString bazi() const;

    void setLanguage(const QString &lang);

    static QString lunarMonthDay(int year, int month, int day);
    static QString holidayAt(int year, int month, int day, const QString &lang = QString());

private:
    LunarCalendarPrivate *d;
    QString m_holidayLang;
};

#endif
