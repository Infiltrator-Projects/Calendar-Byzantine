/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 1993-2026 Shannon Smith
 *
 * Deterministic historical calendar models not supplied by ICU.
 *
 * Revised Julian follows Milanković's 1923 leap rule: ordinary fourth years
 * leap, century years leap only when their remainder modulo 900 is 200 or 600.
 * The arithmetic continuation is anchored to 2000-01-01, when Revised Julian
 * and Gregorian dates coincide.
 *
 * Byzantine Anno Mundi uses the Julian calendar, a 1 September year boundary
 * and the Constantinopolitan creation era whose first year begins 1 September
 * 5509 BC. This resource uses astronomical year numbering internally but
 * formats the historical Anno Mundi year without changing Julian month/day.
 *
 * Egyptian civil dates use Ptolemy's Nabonassar era as the absolute epoch:
 * 1 Thoth, year 1 = JDN 1448638 (26 February 747 BC in the proleptic Julian
 * calendar). This makes the otherwise wandering 365-day Egyptian civil year
 * deterministic. It must not be confused with pharaonic regnal-year dating.
 */

#include "calendar-historical.h"

#include "calendar-helpers.h"
#include "julian-day.h"

#include <infiltratr/arithmetic.h>
#include <glib/gi18n-lib.h>
#include <math.h>

#define floor_divide calendar_floor_divide
#define positive_modulo calendar_positive_modulo
#define gregorian_to_jdn calendar_gregorian_to_jdn
#define jdn_to_julian calendar_jdn_to_julian
#define julian_to_jdn calendar_julian_to_jdn

#define REVISED_JULIAN_ANCHOR_YEAR ((gint64)2000)
#define EGYPTIAN_NABONASSAR_EPOCH_JDN ((gint64)1448638)

static const gchar *const common_months[] = {
    "", N_("January"), N_("February"), N_("March"), N_("April"),
    N_("May"), N_("June"), N_("July"), N_("August"), N_("September"),
    N_("October"), N_("November"), N_("December")
};


void
calendar_byzantine_fields_from_jdn(
    gint64 jdn,
    CalendarCalendarFields *fields)
{
    gint julian_year;
    gint month;
    gint day;

    g_return_if_fail(fields != NULL);

    jdn_to_julian(jdn, &julian_year, &month, &day);
    *fields = (CalendarCalendarFields){
        .year = calendar_i64_add_saturating(
            julian_year, month >= 9 ? 5509 : 5508),
        .month = month,
        .day = day,
        .auxiliary = 0,
        .special = FALSE
    };
}

gint64
calendar_byzantine_fields_to_jdn(
    const CalendarCalendarFields *fields)
{
    gint64 julian_year;

    g_return_val_if_fail(fields != NULL, julian_to_jdn(2000, 1, 1));

    julian_year = calendar_i64_subtract_saturating(
        fields->year, fields->month >= 9 ? 5509 : 5508);
    g_return_val_if_fail(julian_year >= G_MININT &&
                         julian_year <= G_MAXINT,
                         julian_to_jdn(2000, 1, 1));

    return julian_to_jdn((gint)julian_year, fields->month, fields->day);
}

gint
calendar_byzantine_month_length(
    const CalendarCalendarFields *fields)
{
    gint64 julian_year;

    g_return_val_if_fail(fields != NULL, 0);
    g_return_val_if_fail(fields->month >= 1 && fields->month <= 12, 0);

    julian_year = calendar_i64_subtract_saturating(
        fields->year, fields->month >= 9 ? 5509 : 5508);
    return calendar_julian_month_length(
        julian_year, fields->month);
}

gint
calendar_byzantine_period_index(gint month)
{
    g_return_val_if_fail(month >= 1 && month <= 12, 0);
    return month >= 9 ? month - 9 : month + 3;
}

gint
calendar_byzantine_month_from_period(gint period)
{
    g_return_val_if_fail(period >= 0 && period < 12, 9);
    return period < 4 ? period + 9 : period - 3;
}

gchar *
calendar_byzantine_format(
    const CalendarCalendarFields *fields,
    CalendarDatePart part)
{
    g_return_val_if_fail(fields != NULL, g_strdup(""));
    return calendar_format_named_date(fields, part, common_months, G_N_ELEMENTS(common_months), "A.M.");
}

/*
 * Wandering 365-day years use one checked inverse. Saturation would make
 * INT64_MAX appear to round-trip even when the intermediate year*365 or
 * epoch addition overflowed. No provider may mistake that for a date.
 */


/*
 * Traditional Armenian wandering year.
 *
 * The numbered Armenian era begins 11 July 552 in the Julian calendar.
 * Each year has twelve 30-day months followed by five Aweleacʿ epagomenal
 * days and no leap day; this is deliberately separate from Sarkawag's reform.
 */
#define ARMENIAN_EPOCH_JDN (calendar_julian_to_jdn(552, 7, 11))


