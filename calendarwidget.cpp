#include "calendarwidget.h"
#include "lunarcalendar.h"

#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>

CalendarWidget::CalendarWidget(QWidget *parent)
    : QWidget(parent)
    , m_today(QDate::currentDate())
    , m_locale(QLocale())
{
    m_grid = new QGridLayout(this);
    m_grid->setSpacing(1);

    m_holidayLang = QStringLiteral("zh_CN");
    m_selectedDate = m_today;
    setYearMonth(m_today.year(), m_today.month());
}

void CalendarWidget::setYearMonth(int year, int month)
{
    m_year = year;
    m_month = month;
    rebuild();
}

void CalendarWidget::setSelectedDate(const QDate &date)
{
    m_selectedDate = date;
    rebuild();
}

void CalendarWidget::setLocale(const QLocale &locale)
{
    m_locale = locale;
    rebuild();
}

void CalendarWidget::setHolidayLanguage(const QString &lang)
{
    m_holidayLang = lang;
    rebuild();
}

void CalendarWidget::rebuild()
{
    while (m_grid->count() > 0) {
        QLayoutItem *item = m_grid->takeAt(0);
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    Qt::DayOfWeek firstDay = QLocale().firstDayOfWeek();
    int diff = firstDay - Qt::Monday;

    for (int i = 0; i < 7; ++i) {
        int idx = (i + diff + 7) % 7;
        auto *label = new QLabel(m_locale.dayName(idx + 1, QLocale::ShortFormat));
        label->setAlignment(Qt::AlignCenter);
        QFont f = label->font();
        f.setBold(true);
        label->setFont(f);
        m_grid->addWidget(label, 0, i);
    }

    QDate firstOfMonth(m_year, m_month, 1);
    int daysInMonth = firstOfMonth.daysInMonth();
    int startWeekday = firstOfMonth.dayOfWeek();

    int firstOffset = startWeekday - firstDay;
    if (firstOffset < 0) firstOffset += 7;

    for (int day = 1; day <= daysInMonth; ++day) {
        int cellIdx = firstOffset + day - 1;
        int row = cellIdx / 7 + 1;
        int col = cellIdx % 7;

        QDate d(m_year, m_month, day);
        bool isToday = (d == m_today);
        bool isSelected = (d == m_selectedDate);

        QFont df = font();
        df.setBold(true);

        auto *dl = new QLabel(QString::number(day));
        dl->setAlignment(Qt::AlignHCenter);
        dl->setFont(df);

        auto *ll = new QLabel(LunarCalendar::lunarMonthDay(m_year, m_month, day));
        ll->setAlignment(Qt::AlignHCenter);
        QFont lf = ll->font();
        lf.setPointSize(lf.pointSize() - 1);
        ll->setFont(lf);

        QString holiday = LunarCalendar::holidayAt(m_year, m_month, day, m_holidayLang);
        QStringList holidayLines;
        const auto terms = holiday.split(", ");
        for (int ti = 0; ti < terms.size(); ++ti) {
            if (ti > 0) holidayLines << QStringLiteral(",");
            holidayLines << terms[ti].split(' ', Qt::SkipEmptyParts);
        }
        auto *hl = new QLabel(holidayLines.join(QStringLiteral("\n")));
        hl->setAlignment(Qt::AlignHCenter);
        QFont hf = hl->font();
        hf.setPointSize(hf.pointSize() - 2);
        hl->setFont(hf);
        hl->setStyleSheet("color: green;");

        auto *vb = new QVBoxLayout;
        vb->setSpacing(0);
        vb->setContentsMargins(4, 4, 4, 4);
        vb->addWidget(dl);
        vb->addWidget(ll);
        if (!holiday.isEmpty()) vb->addWidget(hl);

        auto *container = new QWidget;
        if (isSelected) {
            QPalette p = container->palette();
            p.setBrush(container->backgroundRole(), p.highlight());
            p.setBrush(container->foregroundRole(), p.highlightedText());
            container->setPalette(p);
            container->setAutoFillBackground(true);
        }
        if (isToday && !isSelected)
            container->setStyleSheet("border:2px solid red;");

        container->setLayout(vb);
        container->setCursor(Qt::PointingHandCursor);
        container->installEventFilter(this);

        container->setProperty("date", d.toString(Qt::ISODate));

        m_grid->addWidget(container, row, col);
    }

    for (int i = 0; i < m_grid->columnCount(); ++i)
        m_grid->setColumnStretch(i, 1);
}

bool CalendarWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        auto *w = qobject_cast<QWidget*>(obj);
        if (w) {
            QString dateStr = w->property("date").toString();
            QDate d = QDate::fromString(dateStr, Qt::ISODate);
            if (d.isValid()) {
                setSelectedDate(d);
                emit dateSelected(d);
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}