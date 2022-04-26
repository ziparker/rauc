#include "stats.h"

RaucStats *r_stats_new(const gchar *label)
{
	RaucStats *stats = g_new0(RaucStats, 1);

	stats->label = g_strdup(label);
	stats->min = G_MAXDOUBLE;
	stats->max = G_MINDOUBLE;

	return stats;
}

void r_stats_add(RaucStats *stats, gdouble value)
{
	g_return_if_fail(stats);

	stats->values[stats->next] = value;
	stats->next = (stats->next + 1) % 64;
	stats->count++;

	stats->sum += value;

	if (value < stats->min)
		stats->min = value;
	if (value > stats->max)
		stats->max = value;
}

gdouble r_stats_get_avg(const RaucStats *stats)
{
	g_return_val_if_fail(stats, 0.0);

	if (stats->count)
		return stats->sum / stats->count;
	else
		return 0.0;
}

gdouble r_stats_get_recent_avg(const RaucStats *stats)
{
	gdouble sum = 0.0;
	guint64 count = stats->count;

	g_return_val_if_fail(stats, 0.0);

	if (count > 64)
		count = 64;

	for (unsigned int i = 0; i < count; i++)
		sum += stats->values[i];

	if (count)
		return sum / count;
	else
		return 0.0;
}

void r_stats_show(const RaucStats *stats, const gchar *prefix)
{
	g_autoptr(GString) msg = g_string_sized_new(128);

	g_return_if_fail(stats);
	g_return_if_fail(prefix);

	g_string_append_printf(msg, "%s: count=%"G_GUINT64_FORMAT, prefix, stats->count);
	if (!stats->count)
		return;
	g_string_append_printf(msg, " sum=%.3f min=%.3f max=%.3f avg=%.3f",
			stats->sum, stats->min, stats->max, r_stats_get_avg(stats));
	g_string_append_printf(msg, " recent-avg=%.3f", r_stats_get_recent_avg(stats));
	g_message("%s", msg->str);
}

void r_stats_free(RaucStats *stats)
{
	if (!stats)
		return;

	g_free(stats->label);

	g_free(stats);
}
