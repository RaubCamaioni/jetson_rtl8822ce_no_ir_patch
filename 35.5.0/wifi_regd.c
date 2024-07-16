/******************************************************************************
 *
 * Copyright(c) 2009-2010 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/

#include <drv_types.h>

#ifdef CONFIG_IOCTL_CFG80211

#include <rtw_wifi_regd.h>

/* 2G chan 01 - chan 11 */
#define RTW_2GHZ_CH01_11 REG_RULE(2412-10, 2462+10, 40, 0, 20, 0)

/* 2G chan 12 - chan 13, PASSIV SCAN */
#define RTW_2GHZ_CH12_13 REG_RULE(2467-10, 2472+10, 40, 0, 20, NL80211_RRF_PASSIVE_SCAN)

/* 5G chan 36 - chan 165 */
#define RTW_5GHZ_5150_5850 REG_RULE(5150-10, 5850+10, 40, 0, 30, NL80211_RRF_PASSIVE_SCAN | NL80211_RRF_NO_IBSS)

static const struct ieee80211_regdomain rtw_regdom_rd = {
	.n_reg_rules = 3,
	.alpha2 = "99",
	.reg_rules = {
		RTW_2GHZ_CH01_11,
		RTW_2GHZ_CH12_13,
		RTW_5GHZ_5150_5850,
	}
};

void rtw_regd_apply_flags(struct wiphy *wiphy)
{
	struct dvobj_priv *dvobj = wiphy_to_dvobj(wiphy);
	struct rf_ctl_t *rfctl = dvobj_to_rfctl(dvobj);
	RT_CHANNEL_INFO *channel_set = rfctl->channel_set;
	u8 max_chan_nums = rfctl->max_chan_nums;

	struct ieee80211_supported_band *sband;
	struct ieee80211_channel *ch;
	unsigned int i, j;
	u16 channel;
	u32 freq;

	/* all channels enable */
	for (i = 0; i < NUM_NL80211_BANDS; i++) {
		sband = wiphy->bands[i];

		if (sband) {
			for (j = 0; j < sband->n_channels; j++) {
				ch = &sband->channels[j];
				ch->flags = 0;
			}
		}
	}

	/* channels apply by channel plans. */
	for (i = 0; i < max_chan_nums; i++) {
		channel = channel_set[i].ChannelNum;
		freq = rtw_ch2freq(channel);

		ch = ieee80211_get_channel(wiphy, freq);
		if (!ch) {
			continue;
		}

		if (channel_set[i].ScanType == SCAN_PASSIVE && rtw_odm_dfs_domain_unknown(dvobj)) {
			// ch->flags = IEEE80211_CHAN_NO_IR;
		}
		else {
			ch->flags = 0;
		}

		if (rtw_is_dfs_ch(ch->hw_value) && rtw_odm_dfs_domain_unknown(dvobj)) {
			// ch->flags |= IEEE80211_CHAN_RADAR;
			// ch->flags |= IEEE80211_CHAN_NO_IR;
		}
	}
}

static const struct ieee80211_regdomain *_rtw_regdomain_select(struct rtw_regulatory *reg)
{
	return &rtw_regdom_rd;
}

static void rtw_reg_notifier(struct wiphy *wiphy, struct regulatory_request *request)
{
	switch (request->initiator) {
	case NL80211_REGDOM_SET_BY_DRIVER:
		RTW_INFO("%s: %s\n", __func__, "NL80211_REGDOM_SET_BY_DRIVER");
		break;
	case NL80211_REGDOM_SET_BY_CORE:
		RTW_INFO("%s: %s\n", __func__, "NL80211_REGDOM_SET_BY_CORE");
		break;
	case NL80211_REGDOM_SET_BY_USER:
		RTW_INFO("%s: %s alpha2:%c%c\n", __func__, "NL80211_REGDOM_SET_BY_USER"
			, request->alpha2[0], request->alpha2[1]);
		rtw_set_country(wiphy_to_adapter(wiphy), request->alpha2);
		break;
	case NL80211_REGDOM_SET_BY_COUNTRY_IE:
		RTW_INFO("%s: %s\n", __func__, "NL80211_REGDOM_SET_BY_COUNTRY_IE");
		break;
	}

	rtw_regd_apply_flags(wiphy);
}

static void _rtw_regd_init_wiphy(struct rtw_regulatory *reg, struct wiphy *wiphy)
{
	const struct ieee80211_regdomain *regd;
	wiphy->reg_notifier = rtw_reg_notifier;
	wiphy->regulatory_flags |= REGULATORY_CUSTOM_REG;
	wiphy->regulatory_flags &= ~REGULATORY_STRICT_REG;
	wiphy->regulatory_flags &= ~REGULATORY_DISABLE_BEACON_HINTS;
	regd = _rtw_regdomain_select(reg);
	wiphy_apply_custom_regulatory(wiphy, regd);
	rtw_regd_apply_flags(wiphy);
}

int rtw_regd_init(struct wiphy *wiphy)
{
	_rtw_regd_init_wiphy(NULL, wiphy);
	return 0;
}
#endif /* CONFIG_IOCTL_CFG80211 */
