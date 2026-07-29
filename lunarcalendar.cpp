#include "lunarcalendar.h"
#include <lunar-date/lunar-date.h>
#include <QDebug>
#include <QStringList>
#include <QHash>
#include <QDate>
#include <KHolidays/HolidayRegion>
#include <KHolidays/Holiday>
#include <clocale>
#include <cstring>
#include <cstdlib>

struct LunarCalendarPrivate {
    LunarDate *date;
};

LunarCalendar::LunarCalendar()
    : d(new LunarCalendarPrivate)
{
    d->date = lunar_date_new();
}

LunarCalendar::~LunarCalendar()
{
    if (d->date)
        lunar_date_free(d->date);
    delete d;
}

void LunarCalendar::setLanguage(const QString &lang)
{
    m_holidayLang = lang;
}

struct FestivalEntry {
    int month;
    int day;
    const char *zh;
    const char *en;
    const char *ru;
};

static const FestivalEntry LUNAR_FESTIVALS[] = {
    {1,  1,  "春节",     "Spring Festival",       "Праздник Весны"},
    {1,  15, "元宵节",   "Lantern Festival",      "Праздник фонарей"},
    {2,  2,  "龙抬头",   "Dragon Head Raising",   "Праздник дракона"},
    {5,  5,  "端午节",   "Dragon Boat Festival",  "Праздник драконьих лодок"},
    {7,  7,  "七夕节",   "Qixi Festival",         "Цисицзе"},
    {7,  15, "中元节",   "Zhongyuan Festival",    "Чжунъюаньцзе"},
    {8,  15, "中秋节",   "Mid-Autumn Festival",   "Праздник середины осени"},
    {9,  9,  "重阳节",   "Double Ninth Festival", "Праздник двойной девятки"},
    {12, 8,  "腊八节",   "Laba Festival",         "Лабацзе"},
};

static QString festivalForLanguage(const FestivalEntry &e, const QString &lang)
{
    if (lang == "en") return QString::fromUtf8(e.en);
    if (lang == "ru") return QString::fromUtf8(e.ru);
    return QString::fromUtf8(e.zh);
}

// KF6Holidays region cache (system locale only)
static KHolidays::HolidayRegion s_systemHolidayRegion;
static bool s_systemHolidayInitialized = false;

static const KHolidays::HolidayRegion &systemHolidayRegion()
{
    if (!s_systemHolidayInitialized) {
        QLocale sys = QLocale::system();
        QString country = QLocale::territoryToString(sys.territory());
        QString lang = sys.bcp47Name().replace('-', '_');
        QString rc = KHolidays::HolidayRegion::defaultRegionCode(country, lang);
        s_systemHolidayRegion = KHolidays::HolidayRegion(rc);
        s_systemHolidayInitialized = true;
    }
    return s_systemHolidayRegion;
}

static void collectKf6Holidays(LunarDate *ld, QStringList &result)
{
    const auto &region = systemHolidayRegion();
    if (!region.isValid()) return;

    gchar *syear = lunar_date_strftime(ld, "%(year)");
    gchar *smonth = lunar_date_strftime(ld, "%(month)");
    gchar *sday = lunar_date_strftime(ld, "%(day)");
    int y = syear ? atoi(syear) : 0;
    int m = smonth ? atoi(smonth) : 0;
    int d = sday ? atoi(sday) : 0;
    g_free(syear); g_free(smonth); g_free(sday);

    if (y == 0 || m == 0 || d == 0) return;

    QDate qdate(y, m, d);
    auto kh = region.rawHolidays(qdate, qdate);
    for (const auto &h : kh) {
        QString name = h.name().trimmed();
        if (!name.isEmpty() && !result.contains(name))
            result << name;
    }
}

static const char *chuxiName(const QString &lang)
{
    if (lang == "en") return "New Year's Eve";
    if (lang == "ru") return "\u041A\u0430\u043D\u0443\u043D \u041D\u043E\u0432\u043E\u0433\u043E \u0433\u043E\u0434\u0430";
    return "\u9664\u5915"; // 除夕
}

static QString lookupTraditionalFestival(LunarDate *ld, const QString &lang)
{
    gchar *yue = lunar_date_strftime(ld, "%(yue)");
    gchar *ri  = lunar_date_strftime(ld, "%(ri)");
    int lm = yue ? atoi(yue) : 0;
    int ld_ = ri ? atoi(ri) : 0;
    g_free(yue);
    g_free(ri);

    // Check lunar festivals
    for (auto &e : LUNAR_FESTIVALS) {
        if (e.month == lm && e.day == ld_)
            return festivalForLanguage(e, lang);
    }

    // Check 除夕: if yesterday was 正月初一, that means today is... 
    // Actually we check: is tomorrow 正月初一?
    gchar *syear = lunar_date_strftime(ld, "%(year)");
    gchar *smonth = lunar_date_strftime(ld, "%(month)");
    gchar *sday = lunar_date_strftime(ld, "%(day)");
    int sy = syear ? atoi(syear) : 0;
    int sm = smonth ? atoi(smonth) : 0;
    int sd = sday ? atoi(sday) : 0;
    g_free(syear); g_free(smonth); g_free(sday);

    if (sy > 0 && sm > 0 && sd > 0) {
        // Get tomorrow's date
        GDate *gd = g_date_new_dmy((GDateDay)sd, (GDateMonth)sm, (GDateYear)sy);
        g_date_add_days(gd, 1);
        int ty = g_date_get_year(gd);
        int tm = g_date_get_month(gd);
        int td = g_date_get_day(gd);
        g_date_free(gd);

        GError *err = nullptr;
        LunarDate *next = lunar_date_new();
        lunar_date_set_solar_date(next, ty, (GDateMonth)tm, td, 0, &err);
        if (!err) {
            gchar *nyue = lunar_date_strftime(next, "%(yue)");
            gchar *nri  = lunar_date_strftime(next, "%(ri)");
            int nlm = nyue ? atoi(nyue) : 0;
            int nld = nri ? atoi(nri) : 0;
            g_free(nyue); g_free(nri);
            if (nlm == 1 && nld == 1) {
                lunar_date_free(next);
                return QString::fromUtf8(chuxiName(lang));
            }
        }
        if (err) g_error_free(err);
        lunar_date_free(next);
    }

    return {};
}

static const char *localeForCode(const QString &code)
{
    if (code == "en") return "en_US.UTF-8";
    if (code == "ru") return "ru_RU.UTF-8";
    return "zh_CN.UTF-8";
}

void LunarCalendar::setSolarDate(int year, int month, int day)
{
    GError *err = nullptr;
    lunar_date_set_solar_date(d->date, year, static_cast<GDateMonth>(month), day, 0, &err);
    if (err) {
        qWarning() << "lunar_date_set_solar_date error:" << err->message;
        g_error_free(err);
    }
}

QString LunarCalendar::lunarYear() const
{
    gchar *s = lunar_date_strftime(d->date, "%(NIAN)");
    QString r = QString::fromUtf8(s);
    g_free(s);
    return r;
}

QString LunarCalendar::lunarMonth() const
{
    gchar *s = lunar_date_strftime(d->date, "%(YUE)");
    QString r = QString::fromUtf8(s);
    g_free(s);
    return r;
}

QString LunarCalendar::lunarDay() const
{
    gchar *s = lunar_date_strftime(d->date, "%(RI)");
    QString r = QString::fromUtf8(s);
    g_free(s);
    return r;
}

QString LunarCalendar::ganzhiYear() const
{
    gchar *s = lunar_date_strftime(d->date, "%(NIAN)");
    QString r = QString::fromUtf8(s);
    g_free(s);
    return r;
}

QString LunarCalendar::ganzhiMonth() const
{
    gchar *s = lunar_date_strftime(d->date, "%(M60)");
    QString r = QString::fromUtf8(s);
    g_free(s);
    return r;
}

QString LunarCalendar::ganzhiDay() const
{
    gchar *s = lunar_date_strftime(d->date, "%(D60)");
    QString r = QString::fromUtf8(s);
    g_free(s);
    return r;
}

QString LunarCalendar::zodiac() const
{
    gchar *s = lunar_date_strftime(d->date, "%(shengxiao)");
    QString r = QString::fromUtf8(s);
    g_free(s);
    return r;
}

QString LunarCalendar::holiday() const
{
    QStringList result;

    // Library holidays (solar terms)
    char *prev = strdup(setlocale(LC_ALL, nullptr));
    setlocale(LC_ALL, localeForCode(m_holidayLang));

    gchar *s = lunar_date_get_holiday(d->date, " ");
    if (s && s[0]) {
        QString h = QString::fromUtf8(s).trimmed();
        if (!h.isEmpty()) result << h;
    }
    g_free(s);

    setlocale(LC_ALL, prev);
    free(prev);

    // Traditional lunar festivals
    QString tf = lookupTraditionalFestival(d->date, m_holidayLang);
    if (!tf.isEmpty() && !result.contains(tf))
        result << tf;

    // Solar holidays from system locale's KF6Holidays
    collectKf6Holidays(d->date, result);

    return result.join(", ");
}

QString LunarCalendar::bazi() const
{
    gchar *s = lunar_date_strftime(d->date, "%(Y8)年%(M8)月%(D8)日%(H8)时");
    QString r = QString::fromUtf8(s);
    g_free(s);
    return r;
}

QString LunarCalendar::constellation() const
{
    gchar *s = lunar_date_get_constellation(d->date);
    QString r = QString::fromUtf8(s);
    g_free(s);
    return r;
}

QString LunarCalendar::lunarMonthDay(int year, int month, int day)
{
    LunarDate *ld = lunar_date_new();
    GError *err = nullptr;
    lunar_date_set_solar_date(ld, year, static_cast<GDateMonth>(month), day, 0, &err);
    if (err) {
        g_error_free(err);
        lunar_date_free(ld);
        return {};
    }
    gchar *ri = lunar_date_strftime(ld, "%(RI)");
    QString result = QString::fromUtf8(ri);
    g_free(ri);
    lunar_date_free(ld);
    return result;
}

QString LunarCalendar::holidayAt(int year, int month, int day, const QString &lang)
{
    LunarDate *ld = lunar_date_new();
    GError *err = nullptr;
    lunar_date_set_solar_date(ld, year, static_cast<GDateMonth>(month), day, 0, &err);
    if (err) {
        g_error_free(err);
        lunar_date_free(ld);
        return {};
    }

    QStringList result;

    char *prev = strdup(setlocale(LC_ALL, nullptr));
    setlocale(LC_ALL, localeForCode(lang));

    gchar *h = lunar_date_get_holiday(ld, " ");
    if (h && h[0]) {
        QString h2 = QString::fromUtf8(h).trimmed();
        if (!h2.isEmpty()) result << h2;
    }
    g_free(h);

    setlocale(LC_ALL, prev);
    free(prev);

    QString tf = lookupTraditionalFestival(ld, lang);
    if (!tf.isEmpty() && !result.contains(tf))
        result << tf;

    collectKf6Holidays(ld, result);

    lunar_date_free(ld);
    return result.join(", ");
}
