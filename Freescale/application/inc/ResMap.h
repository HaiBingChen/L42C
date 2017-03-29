#ifndef _RESMAP_H
#define _RESMAP_H


//#define ResMap_BaseAddr 						0x00000000

//L40A_FontLib_10272114

#define L40A_icon_Base 				0x00000000
#define	L40A_Bar_base  				0x0019BE9C
#define L40A_typeface_base			0x001A4884
#define	L40A_add_base				0x0022EB40
#define L40A_add2_add				0x0022F7C0
#define L40A_payment3_base			0x00232258
#define L40A_typeface_Timenumber1	0x0023F330
#define L40A_add3_base				0x00269AB0
#define L40A_Font					0x00272000
#define L40A_typeface_Timenumber2	0x00360000

 

#define REsMap_icon_Base            0x00000000
#define activity_icon_base 			REsMap_icon_Base
#define APPS_icon_base				REsMap_icon_Base + 0x000136C8 
#define battery_icon_base			REsMap_icon_Base + 0x000168C8 
#define bluetooth_icon_base			REsMap_icon_Base + 0x00019C88 
#define camera_icon_base			REsMap_icon_Base + 0x000246B6
#define charing_icon_base			REsMap_icon_Base + 0x0003405E
#define dial_icon_base			REsMap_icon_Base + 0x000430EA
#define dial2_keyboard_number	REsMap_icon_Base + 0x0005AC6A

#define disconnected_icon_base 	REsMap_icon_Base + 0x0005E2B6
#define ecranmenu_icon_base     REsMap_icon_Base + 0x000636EE
#define findme_icon_base        REsMap_icon_Base + 0x00080F2E 

#define hr_icon_base			REsMap_icon_Base + 0x00087D36
#define main_icon_base			REsMap_icon_Base + 0x00098F36
#define music_icon_base			REsMap_icon_Base + 0x000AB3A6
#define notification_icon_base		REsMap_icon_Base + 0x000B4BA6
#define payment_icon_base			REsMap_icon_Base + 0x000C33A6
#define payment2_icon_base    		REsMap_icon_Base + 0x000F3D0A

#define perfotmonce_icon_base		REsMap_icon_Base + 0x000FF71C
#define Remind_icon_base			REsMap_icon_Base + 0x00116C88 
#define reset_icon_base			REsMap_icon_Base + 0x0013C488	
#define setting_icon_base			REsMap_icon_Base + 0x0014A3A4
#define siri_icon_base			REsMap_icon_Base + 0x00154A9C	

#define sleep_icon_base			REsMap_icon_Base + 0x0015B7DE  
#define SMS_icon_base			REsMap_icon_Base + 0x0016C71E 
#define timer_icon_base			REsMap_icon_Base + 0x0018D030
#define update_icon_base			REsMap_icon_Base + 0x0019091C 
#define volume_icon_base			REsMap_icon_Base + 0x0019511C 



#define activity_32x32_calories     activity_icon_base 
#define activity_32x32_distance 	activity_icon_base + 0x00000800
#define activity_32x32_payment		activity_icon_base + 0x00001000
#define activity_32x32_step 		activity_icon_base + 0x00001800
#define activity_32x32_timer        activity_icon_base + 0x00002000
#define activity_40x40_calories     activity_icon_base + 0x00002800
#define activity_40x40_distance 	activity_icon_base + 0x00003480
#define activity_40x40_sport 		activity_icon_base + 0x00004100

#define activity_40x40_step         activity_icon_base + 0x00004D80
#define activity_40x40_timer		activity_icon_base + 0x00005A00 
#define activity_64x64_calories     activity_icon_base + 0x00006680
#define activity_64x64_distance 	activity_icon_base + 0x00008680
#define activity_64x64_step 		activity_icon_base + 0x0000A680
#define activity_64x64_timer        activity_icon_base + 0x0000C680

#define activity_average_yellow_48x11	activity_icon_base + 0x0000E680
//#define activity_BMP_40x20_char     	activity_icon_base + 0x0000EAA0
#define activity_Calories_35x10_char     activity_icon_base + 0x0000EAA0
#define activity_Calories_76x20_char 	activity_icon_base + 0x0000ED5C
#define activity_kcal_25x10_char     activity_icon_base + 0x0000F93C
#define activity_kcal_34x20_char 	activity_icon_base + 0x0000FB30

#define activity_km_15x10_char         	activity_icon_base + 0x00010080
#define activity_km_28x16_char         	activity_icon_base + 0x000101AC
#define activity_km_40x20_char			activity_icon_base + 0x0001052C 
#define activity_kmh_h_yellow_19x16		activity_icon_base + 0x00010B6C
#define activity_kmh_km_yellow_28x16	activity_icon_base + 0x00010DCC

#define activity_mi_14x10_char         	activity_icon_base + 0x0001114C
#define activity_mi_20x20_char         	activity_icon_base + 0x00011264
#define activity_mi_21x16_char			activity_icon_base + 0x00011584 
#define activity_mih_h_yellow_21x16		activity_icon_base + 0x00011824
#define activity_mih_mi_yellow_21x16	activity_icon_base + 0x00011AC4

#define activity_Minites_35x10_char     activity_icon_base + 0x00011D64
#define activity_Minites_76x20_char 	activity_icon_base + 0x00012020
#define activity_Steps_26x10_char 		activity_icon_base + 0x00012C00
#define activity_Steps_56x20_char       activity_icon_base + 0x00012E08




#define APPS_findme_40x40  		APPS_icon_base       
#define APPS_timer_40x40  		APPS_icon_base   + 0x00000C80
#define APPS_music_40x40  		APPS_icon_base   + 0x00001900
#define APPS_camera_40x40  		APPS_icon_base   + 0x00002580



//#define battery_icon_base			0x000AB9AC//  0x00098000 + 0x000139AC
#define battery_46x24_0				battery_icon_base
#define battery_46x24_1				battery_icon_base +0x000008A0
#define battery_46x24_2				battery_icon_base +0x00001140
#define battery_46x24_3				battery_icon_base +0x000019E0
#define battery_46x24_4				battery_icon_base +0x00002280
#define battery_46x24_red_0			battery_icon_base +0x00002B20



#define bluetooth_48x56_bluetooth   bluetooth_icon_base
#define bluetooth_48x56_fault       bluetooth_icon_base + 0x00001500
#define bluetooth_48x56_selete       bluetooth_icon_base + 0x00002A00
#define bluetooth_48x56_success       bluetooth_icon_base + 0x00003F00
#define bluetooth_72x72_bluetooth       bluetooth_icon_base + 0x00005400
//#define bluetooth_212x71_surface       bluetooth_icon_base + 0x00007C80
#define bluetooth_dot_41x3       	bluetooth_icon_base + 0x00007C80
#define bluetooth_phone_44x72       bluetooth_icon_base + 0x00007D76
#define bluetooth_watch_36x71       bluetooth_icon_base + 0x00009636





#define camera_50x50		camera_icon_base
#define camera_80x80		camera_icon_base + 0x00001388
#define camera_music_bar_131x12		camera_icon_base + 0x00004588
#define camera_picture_164x131		camera_icon_base + 0x000051D0


#define charing_18x32_green		charing_icon_base
#define charing_18x32_red		charing_icon_base + 0x00000480
#define charing_102x48_0		charing_icon_base + 0x00000900
#define charing_102x48_red_0		charing_icon_base + 0x00002F40
#define charing_backgrpund_1_134x43		charing_icon_base + 0x00005580
#define charing_backgrpund_2_44x157		charing_icon_base + 0x00008284 
#define charing_backgrpund_3_46x156		charing_icon_base + 0x0000B87C


#define dail_20x32_search		dial_icon_base
#define dail_32x32_headportrait	dial_icon_base + 0x00000500
#define dail_88x32_char_search	dial_icon_base + 0x00000D00
#define dail_88x32_handup		dial_icon_base + 0x00002300

#define dail_72x72_answer	dial_icon_base + 0x00003900 
#define dail_72x72_diolog	dial_icon_base + 0x00006180
#define dail_72x72_handup	dial_icon_base + 0x00008A00
#define dail_72x72_micgray	dial_icon_base + 0x0000B280
#define dail_72x72_micyellow	dial_icon_base + 0x0000DB00
#define dail_dail_68x40		dial_icon_base + 0x00010380
#define dail_cancel_68x40	dial_icon_base + 0x000118C0
#define dail_greenarrow_16x16		dial_icon_base + 0x00012E00
#define dail_redarrow_16x16			dial_icon_base + 0x00013000 
#define dial_soundoffgray_56x56		dial_icon_base + 0x00013200
#define dail_soundoffwhite_56x56	dial_icon_base + 0x00014A80
#define dail_tool_56x56				dial_icon_base + 0x00016300 


#define dial_0_15x22		dial2_keyboard_number
#define dial_1_15x22		dial2_keyboard_number + 0x00000294
#define dial_2_15x22		dial2_keyboard_number + 0x00000528
#define dial_3_15x22		dial2_keyboard_number + 0x000007BC 
#define dial_4_15x22		dial2_keyboard_number + 0x00000A50
#define dial_5_15x22		dial2_keyboard_number + 0x00000CE4
#define dial_6_15x22		dial2_keyboard_number + 0x00000F78
#define dial_7_15x22		dial2_keyboard_number + 0x0000120C
#define dial_8_15x22		dial2_keyboard_number + 0x000014A0
#define dial_9_15x22		dial2_keyboard_number + 0x00001734

#define dial_background_68x40	dial2_keyboard_number + 0x000019C8
#define dial_plus_10x13			dial2_keyboard_number + 0x00002F08
#define dial_pound_10x13		dial2_keyboard_number + 0x0000300C
#define dial_pound_15x18		dial2_keyboard_number + 0x00003110
#define dial_star_10x13		dial2_keyboard_number + 0x0000332C
#define dial_star_15x18		dial2_keyboard_number + 0x00003430



#define disconnected_48x48	disconnected_icon_base
#define disconnected_163x52	disconnected_icon_base + 0x00001200
//#define disconnected_204x26	disconnected_icon_base + 0x00005438


#define ecran_57x57_sleep   	ecranmenu_icon_base  
#define ecran_57x57_timer 		ecranmenu_icon_base + 0x00001962
#define ecran_57x57_camera 		ecranmenu_icon_base + 0x000032C4
#define ecran_57x57_tool 		ecranmenu_icon_base + 0x00004C26
#define ecran_57x57_fineme		ecranmenu_icon_base + 0x00006588
#define ecran_57x57_set 		ecranmenu_icon_base + 0x00007EEA 
#define ecran_57x57_music 		ecranmenu_icon_base + 0x0000984C
#define ecran_57x57_phonecall	ecranmenu_icon_base + 0x0000B1AE 
#define ecran_57x57_sport 		ecranmenu_icon_base + 0x0000CB10
#define ecran_57x57_clock 		ecranmenu_icon_base + 0x0000E472
#define ecran_57x57_addrlist 	ecranmenu_icon_base + 0x0000FDD4
#define ecran_57x57_mic_siri 	ecranmenu_icon_base + 0x00011736	
#define ecran_57x57_msg 		ecranmenu_icon_base + 0x00013098
#define ecran_57x57_social		ecranmenu_icon_base + 0x000149FA 
//#define ecran_57x57_pay 		ecranmenu_icon_base + 0x0001635C 
#define ecran_57x57_phoneup 	ecranmenu_icon_base + 0x0001635C
#define ecran_57x57_hr 			ecranmenu_icon_base + 0x00017CBE 
#define ecran_92x92_logo 		ecranmenu_icon_base + 0x00019620

#define findme_39x50_location 	findme_icon_base
#define findme_61x94_phone 		findme_icon_base + 0x00000F3C
#define findme_80x80_phone 	findme_icon_base + 0x00003C08


//#define hr_1_149x142		hr_icon_base
//#define hr_2_75x52			hr_icon_base + 0x0000A54
//#define hr_3_79x70			hr_icon_base + 0x0000C3C4
#define hr_1_126x115		hr_icon_base
#define hr_2_60x46			hr_icon_base + 0x00007134
#define hr_3_67x61			hr_icon_base + 0x000086C4
#define hr_icon_48x48		hr_icon_base + 0x0000A6B2

#define hr_BMP_40x20_char	hr_icon_base + 0x0000B8B2
#define hr_char_AGO_40x16 		hr_icon_base + 0x0000BEF2
#define hr_char_ALERT_110x25 	hr_icon_base + 0x0000C3F2
#define hr_char_DAY_40x16 		hr_icon_base + 0x0000D96E
#define hr_char_HOUR_50x16 		hr_icon_base + 0x0000DE6E
#define hr_char_JUSTNOW_95x13 	hr_icon_base + 0x0000E4AE

#define hr_char_MAX_80x25 	hr_icon_base + 0x0000EE54
#define hr_char_MIN_40x16 	hr_icon_base + 0x0000FDF4
#define hr_char_MIN_66x25 	hr_icon_base + 0x000102F4
#define hr_th_char_10x8 	hr_icon_base + 0x00010FD8
#define hr_minus_28x7 		hr_icon_base + 0x00011078





#define ResMap_main_8x8_redround		main_icon_base
#define ResMap_main_8x8_whiteround		main_icon_base + 0x00000080

#define main_8x8_redround			main_icon_base
#define main_8x8_whiteround			main_icon_base + 0x00000080
#define main_16x8_char_BPM			main_icon_base + 0x00000100
#define main_16x16_logo				main_icon_base + 0x00000200
#define main_16x32_bluetooth		main_icon_base + 0x00000400
#define main_16x32_bluetoothgray    main_icon_base + 0x00000800
#define main_16x32_phone			main_icon_base + 0x00000C00

#define main_24x24_logo				main_icon_base + 0x00001000
#define main_26x26_HR				main_icon_base + 0x00001480
#define main_40x40_sport			main_icon_base + 0x000019C8
#define main_64x64_airplanemode			main_icon_base + 0x00002648
#define main_64x64_airplanemodeoff			main_icon_base + 0x00004648
#define main_64x64_brightness			main_icon_base + 0x00006648 
#define main_64x64_soundoff			main_icon_base + 0x00008648
#define main_64x64_soundon			main_icon_base + 0x0000A648
#define main_64x64_vibrator			main_icon_base + 0x0000C648
#define main_134x54_fineme			main_icon_base + 0x0000E648
#define main_char_45x16_today		main_icon_base + 0x00011ED0



#define music_24x24_minus0      music_icon_base 
#define music_24x24_minus1      music_icon_base + 0x00000480
#define music_24x24_plus0      music_icon_base + 0x00000900
#define music_24x24_plus1      music_icon_base + 0x00000D80
#define music_26x16_next0      music_icon_base + 0x00001200
#define music_26x16_next1      music_icon_base + 0x00001540
#define music_26x16_pre0      music_icon_base + 0x00001880
#define music_26x16_pre1      music_icon_base + 0x00001BC0
#define music_88x88_pause      music_icon_base + 0x00001F00
#define music_88x88_play      music_icon_base + 0x00005B80
//#define music_char_connection_138x20      music_icon_base + 0x00009800
//#define music_char_failed_196x20     music_icon_base + 0x0000AD90



//#define notification_icon_base			REsMap_icon_Base + 0x000CDDEE
#define notification_40x40_email     	notification_icon_base
#define notification_40x40_facebook     notification_icon_base + 0x00000C80
#define notification_40x40_gmail     	notification_icon_base + 0x00001900 
#define notification_40x40_hangouts     notification_icon_base + 0x00002580
#define notification_40x40_instagram    notification_icon_base + 0x00003200
#define notification_40x40_Line     	notification_icon_base + 0x00003E80
#define notification_40x40_Linkedin     notification_icon_base + 0x00004B00
#define notification_40x40_messenger    notification_icon_base + 0x00005780
#define notification_40x40_qq     		notification_icon_base + 0x00006400
#define notification_40x40_skype     	notification_icon_base + 0x00007080
#define notification_40x40_snapchat     notification_icon_base + 0x00007D00
#define notification_40x40_twitter     	notification_icon_base + 0x00008980
#define notification_40x40_uber     	notification_icon_base + 0x00009600
#define notification_40x40_viber     	notification_icon_base + 0x0000A280

#define notification_40x40_wechat     	notification_icon_base + 0x0000AF00
#define notification_40x40_whatsApp     notification_icon_base + 0x0000BB80
#define notification_64x64_greenround    notification_icon_base +0x0000C800

//#define payment_icon_base 
#define animaition_lock_45x67   	payment_icon_base
#define animaition_NFC1_95x70   	payment_icon_base + 0x0000178E
#define animaition_unlock_45x67   	payment_icon_base + 0x00004B82
#define animaition_watch_110x160   	payment_icon_base + 0x00006310
#define button_pay_120x48   	payment_icon_base + 0x0000EC90
#define button_right_60x36   	payment_icon_base + 0x00011990
#define button_wrong_60x36   	payment_icon_base + 0x00012A70
#define credit_69x21   	payment_icon_base + 0x00013B50
#define credit_92x27   	payment_icon_base + 0x000146A2
#define debit_69x21   	payment_icon_base + 0x00015A0A

#define debit_92x27   	payment_icon_base + 0x0001655C
#define paid_174x56   	payment_icon_base + 0x000178C4
#define payment_11x32_dot   	payment_icon_base + 0x0001C4E4
#define payment_26x48_dot   	payment_icon_base + 0x0001C7A4

#define payment_backgrond_240x57   	payment_icon_base + 0x0001D164

#define payment_balance_148x24   	payment_icon_base + 0x00023C44
#define payment_chargray_AM_22x16   	payment_icon_base + 0x00025804
#define payment_chargray_PM_22x16   	payment_icon_base + 0x00025AC4

#define payment_chf_35x17   		payment_icon_base + 0x00025D84
#define payment_currency_10x16   	payment_icon_base + 0x0002622A
#define payment_dolar_11x19   	payment_icon_base + 0x0002636A
#define payment_dolar_14x24   	payment_icon_base + 0x0002650C
#define payment_dolar_22x40   	payment_icon_base + 0x000267AC

#define payment_euor_12x16   	payment_icon_base + 0x00026E8C
#define payment_euor_22x40   	payment_icon_base + 0x0002700C
#define payment_icon_40x40   	payment_icon_base + 0x000276EC 
#define payment_icon_57x57   	payment_icon_base + 0x0002836C 
#define payment_minus_4x1   	payment_icon_base + 0x00029CCE
#define payment_minus_14x6   	payment_icon_base + 0x00029CD6
#define payment_minus_17x7   	payment_icon_base + 0x00029D7E
#define payment_plus_9x9   	payment_icon_base + 0x00029E6C
#define payment_plus_20x20   	payment_icon_base + 0x00029F0E
#define payment_plus_25x25   	payment_icon_base + 0x0002A22E

#define payment_RMB_60x24   	payment_icon_base + 0x0002A710

#define payment_signal_01_11x30   	payment_icon_base + 0x0002B250
#define payment_signal_02_13x41   	payment_icon_base + 0x0002B4E4
#define payment_signal_03_15x55   	payment_icon_base + 0x0002B90E
#define payment_signal_04_16x69   	payment_icon_base + 0x0002BF80
#define payment_signal_11_11x30   	payment_icon_base + 0x0002C820
#define payment_signal_12_13x41   	payment_icon_base + 0x0002CAB4
#define payment_signal_13_15x55   	payment_icon_base + 0x0002CEDE
#define payment_signal_14_16x69   	payment_icon_base + 0x0002D550

//#define round_blue_224x51   	payment_icon_base + 0x0002DEAE
//#define round_green_224x51   	payment_icon_base + 0x000337EE
//#define round_green_226x68   	payment_icon_base + 0x0003912E
//#define round_red_224x51   	payment_icon_base + 0x0004093E 
//#define round_red_226x68   	payment_icon_base + 0x0004627E
#define wallet_94x23  	payment_icon_base + 0x0002DDF0
#define wallet_136x25   	payment_icon_base + 0x0002EED4


#define payment2_blue_a224x51_27x51_1 	payment2_icon_base 
#define payment2_blue_a224x51_27x51_2 	payment2_icon_base + 0x00000AC2
#define payment2_blue_a224x51_40x2_line1 payment2_icon_base + 0x00001584
#define payment2_blue_a224x51_40x2_line2 payment2_icon_base + 0x00001624

#define payment2_green_a224x51_27x51_1 		payment2_icon_base + 0x000016C4
#define payment2_green_a224x51_27x51_2 		payment2_icon_base + 0x00002186
#define payment2_green_a224x51_40x2_line1 	payment2_icon_base + 0x00002C48
#define payment2_green_a224x51_40x2_line2 	payment2_icon_base + 0x00002CE8

#define payment2_green_a226x68_30x2_line1 	payment2_icon_base + 0x00002D88
#define payment2_green_a226x68_30x2_line2 	payment2_icon_base + 0x00002E00
#define payment2_green_a226x68_36x68_1 		payment2_icon_base + 0x00002E78
#define payment2_green_a226x68_36x68_2 		payment2_icon_base + 0x00004198

#define payment2_icon_41x41_blueround 	payment2_icon_base + 0x000054B8
#define payment2_icon_41x41_greedround 	payment2_icon_base + 0x000061DA
#define payment2_icon_41x41_redround 	payment2_icon_base + 0x00006EFC

#define payment2_red_a224x51_27x51_1 		payment2_icon_base + 0x00007C1E
#define payment2_red_a224x51_27x51_2 		payment2_icon_base + 0x000086E0
#define payment2_red_a224x51_40x2_line1 		payment2_icon_base + 0x000091A2
#define payment2_red_a224x51_40x2_line2 		payment2_icon_base + 0x00009242

#define payment2_red_a226x68_30x2_line1 	payment2_icon_base + 0x000092E2
#define payment2_red_a226x68_30x2_line2 	payment2_icon_base + 0x0000935A
#define payment2_red_a226x68_36x68_1 	payment2_icon_base + 0x000093D2
#define payment2_red_a226x68_36x68_2 	payment2_icon_base + 0x0000A6F2



#define perfotmonce_46x77_sport     perfotmonce_icon_base
#define perfotmonce_78x80_cup     	perfotmonce_icon_base + 0x00001BAC
#define perfotmonce_216x88_goal     perfotmonce_icon_base + 0x00004C6C
#define perfotmonce_216x88_move     perfotmonce_icon_base + 0x0000E0EC

#define Remind_80x80_alarmclock 	Remind_icon_base
#define Remind_80x80_eat 			Remind_icon_base + 0x00003200
#define Remind_80x80_mail 			Remind_icon_base + 0x00006400
#define Remind_80x80_medicine 	Remind_icon_base + 0x00009600
#define Remind_80x80_meeting 	Remind_icon_base + 0x0000C800
#define Remind_80x80_missedcall 	Remind_icon_base + 0x0000FA00
#define Remind_80x80_msg 			Remind_icon_base + 0x00012C00
#define Remind_80x80_schedule 		Remind_icon_base + 0x00015E00
#define Remind_80x80_sleep 			Remind_icon_base + 0x00019000
#define Remind_80x80_social 		Remind_icon_base + 0x0001C200
#define Remind_80x80_sport 			Remind_icon_base + 0x0001F400
#define Remind_80x80_udate 			Remind_icon_base + 0x00022600




#define logo_25x27		reset_icon_base
#define logo_34x34		reset_icon_base + 0x00000546
#define logochar_186x92_char_186x24		reset_icon_base + 0x00000E4E
#define logochar_186x92_red_16x21		reset_icon_base + 0x0000312E
#define logochar_186x92_red_27x38		reset_icon_base + 0x000033CE


#define reset_32x32_arrow		reset_icon_base + 0x00003BD2 
#define reset_32x56_signal		reset_icon_base + 0x000043D2
#define reset_44x85_watch		reset_icon_base + 0x000051D2 
#define reset_48x84_phone		reset_icon_base + 0x00006F0A
#define reset_84x52_right		reset_icon_base + 0x00008E8A
#define reset_84x52_wrong		reset_icon_base +  0x0000B0AA
//#define reset_char1_126x51		reset_icon_base + 0x00012AF6 
//#define reset_char2_126x51		reset_icon_base + 0x00015D2A
#define setup_char_83x19		reset_icon_base +  0x0000D2CA 




#define setting_32x32_bluetooth		setting_icon_base 
#define setting_32x32_brightness		setting_icon_base + 0x00000800
#define setting_32x32_clockgray		setting_icon_base + 0x00001000
#define setting_32x32_dollargray		setting_icon_base + 0x00001800
#define setting_32x32_dollargreen		setting_icon_base + 0x00002000
#define setting_32x32_euro		setting_icon_base + 0x00002800
#define setting_32x32_language		setting_icon_base +  0x00003000 
#define setting_32x32_reset		setting_icon_base + 0x00003800
#define setting_32x32_soundblue		setting_icon_base + 0x00004000
#define setting_32x32_soundred		setting_icon_base + 0x00004800
#define setting_32x32_volume		setting_icon_base + 0x00005000
#define setting_32x32_watchinfo		setting_icon_base + 0x00005800
#define setting_32x32_wristflick		setting_icon_base + 0x00006000
#define setting_32x32_yuan		setting_icon_base + 0x00006800 
#define setting_80x80_brightness		setting_icon_base + 0x00007000
#define setting_240x1_Line		setting_icon_base + 0x0000A200
#define setting_right_22x18		setting_icon_base + 0x0000A3E0


#define siri_arc_l1_8x28	siri_icon_base			
#define siri_arc_l2_11x51	siri_icon_base + 0x000001C0
#define siri_arc_l3_15x78	siri_icon_base + 0x00000622
#define siri_arc_r1_8x28	siri_icon_base + 0x00000F46
#define siri_arc_r2_11x51	siri_icon_base + 0x00001106
#define siri_arc_r3_15x78	siri_icon_base + 0x00001568 
#define siri_mic_49x75		siri_icon_base + 0x00001E8C
#define siri_mic_80x80	siri_icon_base + 0x00003B42
 

#define sleep_32x32_Awake   			sleep_icon_base
#define sleep_32x32_AwakeSleep   		sleep_icon_base +  0x00000800
#define sleep_32x32_Deepsleepblue   	sleep_icon_base +  0x00001000
#define sleep_32x32_FellAsleep   		sleep_icon_base +  0x00001800
#define sleep_32x32_Lightsleeppurple   	sleep_icon_base +  0x00002000
#define sleep_32x32_TotalSleep   		sleep_icon_base +  0x00002800
#define sleep_48x48_minus   		sleep_icon_base +  0x00003000 
#define sleep_48x48_plus   			sleep_icon_base +  0x00004200
#define sleep_64x64_sleep   			sleep_icon_base +  0x00005400
#define sleep_88x40_menu_end   		sleep_icon_base + 0x00007400
#define sleep_88x40_menu_ok   		sleep_icon_base + 0x00008F80 
#define sleep_88x40_menu_start   	sleep_icon_base + 0x0000AB00 
#define sleep_char_8x16_h   		sleep_icon_base + 0x0000C680 
#define sleep_char_26x16_min   		sleep_icon_base + 0x0000C780 
#define sleep_char_50x16_awake   	sleep_icon_base + 0x0000CAC0 
#define sleep_char_70x16_lastnight   	sleep_icon_base + 0x0000D100  
#define sleep_char_70x16_totalsleep   	sleep_icon_base + 0x0000D9C0
#define sleep_char_82x16_lightsleep   	sleep_icon_base + 0x0000E280
#define sleep_char_84x16_deepsleep   	sleep_icon_base + 0x0000ECC0
#define sleep_char_96x16_awakesleep   	sleep_icon_base + 0x0000F740
#define sleep_char_96x16_fellasleepin   sleep_icon_base + 0x00010340



#define SMS_114x54_reply      			SMS_icon_base
#define SMS_face_48x48_00_kun      		SMS_icon_base + 0x00003018
#define SMS_face_48x48_01_weiqu      	SMS_icon_base + 0x00004218
#define SMS_face_48x48_02_nu      		SMS_icon_base + 0x00005418
#define SMS_face_48x48_03_yanjing      	SMS_icon_base + 0x00006618
#define SMS_face_48x48_04_liukoushui      	SMS_icon_base + 0x00007818
#define SMS_face_48x48_05_se      		SMS_icon_base + 0x00008A18
#define SMS_face_48x48_06_laotou      	SMS_icon_base + 0x00009C18
#define SMS_face_48x48_07_chijing      	SMS_icon_base + 0x0000AE18
#define SMS_face_48x48_08_kaixin      	SMS_icon_base + 0x0000C018
#define SMS_face_48x48_09_xingxingyan      	SMS_icon_base + 0x0000D218
#define SMS_face_48x48_10_le      		SMS_icon_base + 0x0000E418
#define SMS_face_48x48_11_bizui      	SMS_icon_base + 0x0000F618
#define SMS_face_48x48_12_ku      		SMS_icon_base + 0x00010818
#define SMS_face_48x48_13_nanguo      	SMS_icon_base + 0x00011A18 
#define SMS_face_48x48_14_zhayan      	SMS_icon_base + 0x00012C18
#define SMS_face_48x48_15_xiao      	SMS_icon_base + 0x00013E18
#define SMS_48x48_mic      				SMS_icon_base + 0x00015018
#define SMS_face_80x80_xiao_gray      	SMS_icon_base + 0x00016218


#define SMS_msg_107x71      	SMS_icon_base + 0x00019418
#define SMS_msgfailed_52x52      	SMS_icon_base + 0x0001CF72
#define SMS_msgsented_52x52      	SMS_icon_base + 0x0001E492
#define SMS_msgsenting_48x41      	SMS_icon_base + 0x0001F9B2





#define Timer_char_AM_40x24	timer_icon_base
#define Timer_char_PM_40x24	timer_icon_base + 0x00000780
#define timer_47x58_flag	timer_icon_base + 0x00000F00
#define timer_48x55_timer	timer_icon_base + 0x0000244C



#define update_32x32_progress_gray		update_icon_base
#define update_64x64_complete			update_icon_base + 0x00000800 
#define update_64x64_failed				update_icon_base + 0x00002800 
//#define update_char_198x52_progress		update_icon_base + 0x00004800
//#define update_char_198x52_complete		update_icon_base + 0x00009870
//#define update_char_198x52_failed		update_icon_base + 0x0000E8E0

 
#define volume_40x40_minus		volume_icon_base
#define volume_40x40_plus		volume_icon_base + 0x00000C80
#define volume_104x104_volume	volume_icon_base + 0x00001900


//#define L40A_typeface_base			0x0025D374//0x0021A72A
#define ActivitySportNumber_30x40          L40A_typeface_base
#define ActivitySportNumber_20x32          L40A_typeface_base +  0x00006360
//#define blueweek_60x32          L40A_typeface_base +  0x000097A0
#define Char_9x16_A_Z          	L40A_typeface_base +  0x000097A0
#define Char_10x16_a_z          L40A_typeface_base +  0x0000B7A0 
#define Char_12x16_A_Z          L40A_typeface_base +  0x0000DCE4
#define Char_12x16_a_zs         L40A_typeface_base +  0x00010444
#define Char_17x24_a_z          L40A_typeface_base +  0x000129AC
#define Char_20x24_A_Z          L40A_typeface_base +  0x000180CA





#define ecranmenu_date_number_6x16     L40A_typeface_base +  0x0001DD6A
#define ecranmenu_time32px_23x32       L40A_typeface_base +  0x0001E5AA
#define ecranmenu_time32pxs_23x32      L40A_typeface_base +  0x00021F2A
#define ecranmenu_week_char_23x9       L40A_typeface_base +  0x000258AA
#define graynumber_9x16          		L40A_typeface_base + 0x000263FC 

#define hr_9x16_Number					L40A_typeface_base +  0x00026FFC
//#define Hr_month_37x16          		L40A_typeface_base +  0x00034852
//#define hr_number_9x16          		L40A_typeface_base +  0x00037FD2
#define Hr_yellowmonth_37x16          	L40A_typeface_base +  0x00027CFC
#define hr_yellownumber_9x16          	L40A_typeface_base +  0x0002B47C
#define hr32px_number_17x32          	L40A_typeface_base +  0x0002C05C

#define main_battary_number_12x16       L40A_typeface_base +  0x0002EADC
#define main_battary_18x16_percent  main_battary_number_12x16 + 0x00000F00

#define notification_20x33_number          L40A_typeface_base +  0x0002FC1C

#define number16px_10x16       			L40A_typeface_base +  0x00032FAC 
#define number24px_bold_15x24          	L40A_typeface_base +  0x00033C2C
#define number24pxs_ceriph_15x24        L40A_typeface_base +  0x0003584C
#define number56px_bold_32x56          	L40A_typeface_base +  0x0003746C
#define number56pxs_ceriph_32x56        L40A_typeface_base +  0x0004092C

#define payment_16x24_keyboardnumber       L40A_typeface_base +  0x0004952C
#define payment_20x32_bold_number          L40A_typeface_base +  0x0004B32C
#define payment_20x32_ceriph_number        L40A_typeface_base +  0x0004EA2C
#define payment_26x40_bold_number          L40A_typeface_base +  0x0005212C
#define payment_26x40_ceriph_number        L40A_typeface_base +  0x0005726C
#define payment_13x40_dot         payment_26x40_ceriph_number +  0x00005140

#define payment_30x48_bold_number          L40A_typeface_base +  0x0005C7BC
#define payment_30x48_ceriph_number        L40A_typeface_base +  0x000641FC 
#define payment_graynumber_18x24           L40A_typeface_base +  0x0006B27C 
#define payment_graynumber_18x24_Dot       payment_graynumber_18x24 + 0x000021C0  
#define payment_number_14x24        L40A_typeface_base +  0x0006D79C
#define payment_week_50x24          L40A_typeface_base +  0x0006F1DC 


#define phone_number_11x16          L40A_typeface_base +  0x0007337C
#define sleep_12x16_number          L40A_typeface_base +   0x0007421C 
#define sleep_20x32_number          L40A_typeface_base +  0x0007511C

#define sport_number_18x24          	L40A_typeface_base +  0x0007831C
#define step_number_9x16          		L40A_typeface_base +  0x0007AEFC



#define time24px_12x24_number       L40A_typeface_base +  0x0007BA3C
#define time48px_28x48_number       L40A_typeface_base +  0x0007D0BC
#define timer_number_10x16			L40A_typeface_base +  0x000839BC
#define timer_number_10x16_colon		timer_number_10x16 + 0x00000C80
#define timer_number_10x16_msecond		timer_number_10x16 + 0x00000DC0
#define timer_number_24x40          L40A_typeface_base +  0x000848BC 
#define timer_number_24x40_colon    	timer_number_24x40 + 0x00004B00
#define timer_number_24x40_second   	timer_number_24x40 + 0x00005280


#define colordate_icon_number          	L40A_typeface_Timenumber1
#define colordate_44x48_greenround     		colordate_icon_number
#define colordate_44x48_orangeround     	colordate_icon_number + 0x00001080
#define colordate_44x48_purpleround     	colordate_icon_number + 0x00002100
#define colordate_green_number_12x24     	colordate_icon_number + 0x00003180
#define colordate_orange_number_12x24     	colordate_icon_number + 0x00004800
#define colordate_purple_number_12x24     	colordate_icon_number + 0x00005E80
#define time72px_50x72_number          	L40A_typeface_Timenumber1 +  0x00007500
#define time72pxs_50x72_number          L40A_typeface_Timenumber1 +  0x00018E40

#define time40px_12x40_number          	L40A_typeface_Timenumber2
#define time128px_44x128_number         L40A_typeface_Timenumber2 + 0x00002580








#define activity_barH_a168x10   L40A_Bar_base
#define activity_barH_a210x12   L40A_Bar_base + 0x00000EB0
#define activity_barV_a12x118   L40A_Bar_base + 0x00002D10
#define dail_barV_a6x176   		L40A_Bar_base + 0x00004390
#define hr_barH_a210x12   		L40A_Bar_base + 0x00004750
#define sleep_barH_a210x12   	L40A_Bar_base + 0x00004EE8
#define update_barH_a194x11   	L40A_Bar_base + 0x00005680
#define volume_bar_108x9   		L40A_Bar_base + 0x00005C58



#define activity_barH_blue1_bl_15x12	activity_barH_a210x12
#define activity_barH_blue2_b_18x12	    activity_barH_a210x12 + 0x00000168
#define activity_barH_blue3_br_15x12	activity_barH_a210x12 + 0x00000318
#define activity_barH_blue4_t_18x12	    activity_barH_a210x12 + 0x00000480
#define activity_barH_blue5_tr_15x12	activity_barH_a210x12 + 0x00000630
#define activity_barH_green1_bl_15x12		activity_barH_a210x12 + 0x00000798
#define activity_barH_green2_b_18x12	    activity_barH_a210x12 + 0x00000900
#define activity_barH_green3_br_15x12		activity_barH_a210x12 + 0x00000AB0
#define activity_barH_green4_t_18x12	    activity_barH_a210x12 + 0x00000C18
#define activity_barH_green5_tr_15x12		activity_barH_a210x12 + 0x00000DC8
#define activity_barH_red1_bl_15x12		activity_barH_a210x12 + 0x00000F30
#define activity_barH_red2_b_18x12	    activity_barH_a210x12 + 0x00001098
#define activity_barH_red3_br_15x12		activity_barH_a210x12 + 0x00001248
#define activity_barH_red4_t_18x12	    activity_barH_a210x12 + 0x000013B0
#define activity_barH_red5_tr_15x12		activity_barH_a210x12 + 0x00001560
#define activity_barH_yellow1_bl_15x12		activity_barH_a210x12 + 0x000016C8
#define activity_barH_yellow2_b_18x12	    activity_barH_a210x12 + 0x00001830
#define activity_barH_yellow3_br_15x12		activity_barH_a210x12 + 0x000019E0
#define activity_barH_yellow4_t_18x12	    activity_barH_a210x12 + 0x00001B48
#define activity_barH_yellow5_tr_15x12		activity_barH_a210x12 + 0x00001CF8


#define activity_barH_blue1_bl_9x10		activity_barH_a168x10
#define activity_barH_blue2_b_10x10	    activity_barH_a168x10 + 0x000000B4
#define activity_barH_blue3_br_9x10		activity_barH_a168x10 + 0x0000017C
#define activity_barH_blue4_t_10x10	    activity_barH_a168x10 + 0x00000230
#define activity_barH_blue5_tr_9x10		activity_barH_a168x10 + 0x000002F8
#define activity_barH_green1_bl_9x10		activity_barH_a168x10 + 0x000003AC
#define activity_barH_green2_b_10x10	    activity_barH_a168x10 + 0x00000460
#define activity_barH_green3_br_9x10		activity_barH_a168x10 + 0x00000528
#define activity_barH_green4_t_10x10	    activity_barH_a168x10 + 0x000005DC
#define activity_barH_green5_tr_9x10		activity_barH_a168x10 + 0x000006A4
#define activity_barH_red1_bl_9x10		activity_barH_a168x10 + 0x00000758
#define activity_barH_red2_b_10x10	    activity_barH_a168x10 + 0x0000080C
#define activity_barH_red3_br_9x10		activity_barH_a168x10 + 0x000008D4
#define activity_barH_red4_t_10x10	    activity_barH_a168x10 + 0x00000988
#define activity_barH_red5_tr_9x10		activity_barH_a168x10 + 0x00000A50
#define activity_barH_yellow1_bl_9x10		activity_barH_a168x10 + 0x00000B04
#define activity_barH_yellow2_b_10x10	    activity_barH_a168x10 + 0x00000BB8
#define activity_barH_yellow3_br_9x10		activity_barH_a168x10 + 0x00000C80
#define activity_barH_yellow4_t_10x10	    activity_barH_a168x10 + 0x00000D34
#define activity_barH_yellow5_tr_9x10		activity_barH_a168x10 + 0x00000DFC


#define activity_barV_blue1_bd_12x14	activity_barV_a12x118
#define activity_barV_blue2_b_12x9		activity_barV_a12x118 + 0x00000150
#define activity_barV_blue3_bu_12x14	activity_barV_a12x118 + 0x00000228
#define activity_barV_blue4_t_12x9		activity_barV_a12x118 + 0x00000378
#define activity_barV_blue5_tu_12x14	activity_barV_a12x118 + 0x00000450
#define activity_barV_green1_bd_12x14		activity_barV_a12x118 + 0x000005A0
#define activity_barV_green2_b_12x9			activity_barV_a12x118 + 0x000006F0
#define activity_barV_green3_bu_12x14		activity_barV_a12x118 + 0x000007C8
#define activity_barV_green4_t_12x9			activity_barV_a12x118 + 0x00000918
#define activity_barV_green5_tu_12x14		activity_barV_a12x118 + 0x000009F0
#define activity_barV_red1_bd_12x14		activity_barV_a12x118 + 0x00000B40
#define activity_barV_red2_b_12x9		activity_barV_a12x118 + 0x00000C90
#define activity_barV_red3_bu_12x14		activity_barV_a12x118 + 0x00000D68
#define activity_barV_red4_t_12x9		activity_barV_a12x118 + 0x00000EB8
#define activity_barV_red5_tu_12x14		activity_barV_a12x118 + 0x00000F90
#define activity_barV_yellow1_bd_12x14		activity_barV_a12x118 + 0x000010E0
#define activity_barV_yellow2_b_12x9		activity_barV_a12x118 + 0x00001230
#define activity_barV_yellow3_bu_12x14		activity_barV_a12x118 + 0x00001308
#define activity_barV_yellow4_t_12x9		activity_barV_a12x118 + 0x00001458
#define activity_barV_yellow5_tu_12x14		activity_barV_a12x118 + 0x00001530


#define dail_barV_1_b_up_6x10		dail_barV_a6x176
#define dail_barV_2_b_6x10 			dail_barV_a6x176 + 0x00000078
#define dail_barV_3_b_down_6x10 		dail_barV_a6x176 + 0x000000F0
#define dail_barV_4_t_upbalck_6x10 		dail_barV_a6x176 + 0x00000168
#define dail_barV_5_t_6x10				dail_barV_a6x176 + 0x000001E0
#define dail_barV_6_t_downgray_6x10 	dail_barV_a6x176 + 0x00000258
#define dail_barV_7_t_upgray_6x10 		dail_barV_a6x176 + 0x000002D0
#define dail_barV_8_t_downblack_6x10 	dail_barV_a6x176 + 0x00000348


#define update_barH_green1_bl_12x11 	update_barH_a194x11
#define update_barH_green2_b_10x11 		update_barH_a194x11 + 0x00000108
#define update_barH_green3_br_12x11 	update_barH_a194x11 + 0x000001E4
#define update_barH_green4_tl_12x11 	update_barH_a194x11 + 0x000002EC
#define update_barH_green5_t_10x11 		update_barH_a194x11 + 0x000003F4
#define update_barH_green6_tr_12x11 	update_barH_a194x11 + 0x000004D0


#define volume_108x9_bar_5  volume_bar_108x9
#define volume_108x9_bar_4  volume_bar_108x9 + 0x00000798
#define volume_108x9_bar_3  volume_bar_108x9 + 0x00000F30
#define volume_108x9_bar_2  volume_bar_108x9 + 0x000016C8
#define volume_108x9_bar_1  volume_bar_108x9 + 0x00001E60
#define volume_108x9_bar_0  volume_bar_108x9 + 0x000025F8


#define hr_barH_red1_bl_15x12	hr_barH_a210x12
#define hr_barH_red2_b_18x12	hr_barH_a210x12 + 0x00000168
#define hr_barH_red3_br_15x12	hr_barH_a210x12 + 0x00000318
#define hr_barH_red4_t_18x12	hr_barH_a210x12 + 0x00000480
#define hr_barH_red5_tr_15x12	hr_barH_a210x12 + 0x00000630


#define sleep_barH_purple1_bl_15x12	sleep_barH_a210x12
#define sleep_barH_purple2_b_18x12	sleep_barH_a210x12 + 0x00000168
#define sleep_barH_purple3_br_15x12	sleep_barH_a210x12 + 0x00000318
#define sleep_barH_purple4_t_18x12	sleep_barH_a210x12 + 0x00000480
#define sleep_barH_purple5_tr_15x12	sleep_barH_a210x12 + 0x00000630



//add
#define char_colon_4x16				L40A_add_base 
#define char_minus_6x16				L40A_add_base + 0x00000080
#define char_slash_6x16				L40A_add_base + 0x00000140
#define dial_pound_gray_9x16	L40A_add_base + 0x00000200
#define dial_pound_white_9x16	L40A_add_base + 0x00000320
#define hr_char_nd_12x8			L40A_add_base + 0x00000440
#define hr_char_rd_10x8			L40A_add_base + 0x00000500
#define hr_char_st_10x8         L40A_add_base + 0x000005A0 


#define payment_currency_CHF_26x16		L40A_add_base + 0x00000640
#define payment_currency_dollar_8x16	L40A_add_base + 0x00000980
#define payment_currency_euro_8x16		L40A_add_base + 0x00000A80
#define payment_currency_f_8x16			L40A_add_base + 0x00000B80
 



#if 0
//L40A_setting2_language_base
//add_setting2_icon_language
#define setting_lan_cn_sim_160x19 	L40A_setting2_language_base 
#define setting_lan_cn_tra_164x19 	L40A_setting2_language_base + 0x000017C0
#define setting_lan_deutsch_70x19 	L40A_setting2_language_base + 0x00003018
#define setting_lan_dutch_50x19 	L40A_setting2_language_base + 0x00003A7C
#define setting_lan_english_62x19 	L40A_setting2_language_base + 0x000041E8
#define setting_lan_espanol_68x19 	L40A_setting2_language_base + 0x00004B1C
#define setting_lan_franc_73x19 	L40A_setting2_language_base + 0x00005534
#define setting_lan_itliano_61x19 	L40A_setting2_language_base + 0x0000600A
#define setting_lan_japanese_57x19 	L40A_setting2_language_base + 0x00006918
#define setting_lan_Korean_57x19 	L40A_setting2_language_base + 0x0000718E
#define setting_lan_latin_64x19 	L40A_setting2_language_base + 0x00007A04
#define setting_lan_polski_49x19 	L40A_setting2_language_base + 0x00008384
#define setting_lan_portugues_87x19 	L40A_setting2_language_base + 0x00008ACA 
#define setting_lan_Russian_152x19 	L40A_setting2_language_base + 0x000097B4
#endif


//L40A_add2_add



#define dial_greenarrow_16x16_1 			L40A_add2_add
#define dial_pound_12x24 				L40A_add2_add + 0x00000200
#define dial_redarrow_16x16_1 			L40A_add2_add + 0x00000440
#define dial_star_12x24 				L40A_add2_add + 0x00000640
#define hr_char_dot_2x16 					L40A_add2_add + 0x00000880
#define setting_lan_japanese_57x19			L40A_add2_add + 0x000008C0
#define setting_lan_japanese_57x19_gray 	L40A_add2_add + 0x00001136
#define setting_lan_Korean_57x19 			L40A_add2_add + 0x000019AC
#define setting_lan_Korean_57x19_gray 		L40A_add2_add + 0x00002222



//payment3 LOG
#define pay1_cahr_REQUIRED_107x16		L40A_payment3_base
#define pay1_cahr_SET_38x16		L40A_payment3_base + 0x00000D60
#define pay1_cahr_UP_29x16		L40A_payment3_base + 0x00001220
#define pay1_cahr1_K_14x18		L40A_payment3_base + 0x000015C0
#define pay1_cahr1_M_18x18		L40A_payment3_base + 0x000017B8

#define pay1_cahr1_N_16x18		L40A_payment3_base + 0x00001A40
#define pay1_cahr1_O_18x18		L40A_payment3_base + 0x00001C80
#define pay1_cahr1_R_14x18		L40A_payment3_base + 0x00001F08
#define pay1_cahr1_Y_16x18		L40A_payment3_base + 0x00002100
#define pay1_cahr1_Z_13x18		L40A_payment3_base + 0x00002340

#define pay1_logo1_1_26x26		L40A_payment3_base + 0x00002514
#define pay1_logo1_2_27x74		L40A_payment3_base + 0x00002A5C
#define pay1_logo1_3_21x26		L40A_payment3_base + 0x000039F8
#define pay1_logo1_PAY_117x50		L40A_payment3_base + 0x00003E3C
#define pay2_cahr1_in_16x16		L40A_payment3_base + 0x00006BF0

#define pay2_cahr1_progress_109x16		L40A_payment3_base + 0x00006DF0
#define pay2_cahr1_Transaction_118x16	L40A_payment3_base + 0x00007B90
#define pay2_logo1_1_15x15		L40A_payment3_base + 0x00008A50
#define pay2_logo1_2_16x43		L40A_payment3_base + 0x00008C12
#define pay2_logo1_3_12x15		L40A_payment3_base + 0x00009172

#define pay2_logo1_MYkronoz_82x11		L40A_payment3_base + 0x000092DA
#define pay2_logo1_PAY_69x29	L40A_payment3_base + 0x000099E6
#define pay2_mostercard1_68x42	L40A_payment3_base + 0x0000A988
#define pay2_mostercard2_64x9	L40A_payment3_base + 0x0000BFD8
#define pay2_round1_20x20		L40A_payment3_base + 0x0000C458

#define pay2_round2_20x20		L40A_payment3_base + 0x0000C778
#define pay2_round3_20x20		L40A_payment3_base + 0x0000CA98
#define pay2_round4_20x20		L40A_payment3_base + 0x0000CDB8


//L40A_add3_base

#define dail_gaynumber_11x16_0			L40A_add3_base
#define dail_gaynumber_11x16_1			L40A_add3_base + 0x00000160
#define dail_gaynumber_11x16_2			L40A_add3_base + 0x000002C0
#define dail_gaynumber_11x16_3			L40A_add3_base + 0x00000420
#define dail_gaynumber_11x16_4			L40A_add3_base + 0x00000580
#define dail_gaynumber_11x16_5			L40A_add3_base + 0x000006E0
#define dail_gaynumber_11x16_6			L40A_add3_base + 0x00000840
#define dail_gaynumber_11x16_7			L40A_add3_base + 0x000009A0
#define dail_gaynumber_11x16_8			L40A_add3_base + 0x00000B00
#define dail_gaynumber_11x16_9			L40A_add3_base + 0x00000C60
#define dail_gaynumber_11x16_plus			L40A_add3_base + 0x00000DC0
#define dail_gaynumber_11x16_pound			L40A_add3_base + 0x00000F20
#define dail_gaynumber_11x16_star			L40A_add3_base + 0x00001080


#define dial_plus_12x24					L40A_add3_base + 0x000011E0
#define Number_18x40_colon				L40A_add3_base + 0x00001420
#define payment_11x13_keyboard_plus		L40A_add3_base + 0x000019C0
#define payment_11x13_keyboard_pound	L40A_add3_base + 0x00001ADE
#define payment_11x13_keyboard_star		L40A_add3_base + 0x00001BFC




#define L42C_icon_Base          0x00000000
#define L42C_Char_base          0x000CF234
#define L42C_typeface_base      0x000E02BE
#define L42C_bar_base           0x000F02F4
#define timeblack_base          0x000F1358
#define timestar_base           0x0010B208 
#define timesunup_base          0x00134064
#define L42C_ANCS_BG1_icon		0x001611C8
#define L42_add_icon			0x0016AE78
//#define L42C_ANCS_BG_icon		0x001611C8

#define time1_dots_base         0x001A4904
#define timeblue_base			0x001A4F6C
#define timesierra_base			0x001D27A4

#define L53B_Font_161026        0x00272000



#define L42D_bp_icon_base  		0x0016A7A8
#define L42D_bt2_bicon_base     0x00173C28
#define L42D_dail_icon_base     0x001788C8
//#define L42C_ANCS_BG1_icon		0x00188A98


#define	L42C_activity_icon  	L42C_icon_Base
#define L42C_app_icon			L42C_icon_Base + 0x000063A0
#define L42C_app2_icon			L42C_icon_Base + 0x0001A440

#define L42C_camara_icon		L42C_icon_Base + 0x000284E8
#define	L42C_charing_battery	L42C_icon_Base + 0x0002F1FE
#define L42C_connect_icon		L42C_icon_Base + 0x0003A29E
#define L42C_hr_icon			L42C_icon_Base + 0x0003D236
#define L42C_logo_icon			L42C_icon_Base + 0x0005536E
#define L42C_music_icon	        L42C_icon_Base + 0x00059076
#define L42C_notification_icon	L42C_icon_Base + 0x0005E6B6
#define L42C_remind_icon		L42C_icon_Base + 0x0007E936
#define L42C_remind2_icon		L42C_icon_Base + 0x000BBBEE
#define L42C_sleep_icon			L42C_icon_Base + 0x000C9056





#define activity_calorie_48x48 		L42C_activity_icon
#define activity_distance_48x48 	L42C_activity_icon + 0x00001200
#define activity_sporttime_48x48 	L42C_activity_icon + 0x00002400
#define activity_step_48x48		 	L42C_activity_icon + 0x00003600
#define uints_char_Cal_30x16 		L42C_activity_icon + 0x00004800
#define uints_char_Kcal_37x16 	L42C_activity_icon + 0x00004BC0
#define uints_char_Km_30x16 	L42C_activity_icon + 0x00005060
#define uints_char_Mi_24x16 		L42C_activity_icon + 0x00005420
#define uints_char_Min_35x16 	L42C_activity_icon + 0x00005720
#define uints_char_Steps_52x20 	L42C_activity_icon + 0x00005B80



#define app_activity_90x90		 	L42C_app_icon
#define app_main_82x77		 		L42C_app_icon + 0x00003F48
#define app_music_40x40		 		L42C_app_icon + 0x0000709C 
#define app_pair_greenrond_38x38	L42C_app_icon + 0x00007D1C
#define app_pair_redrond_38x38		L42C_app_icon + 0x00008864
#define app_photos_40x40			L42C_app_icon + 0x000093AC
#define app_reminder_40x40			L42C_app_icon + 0x0000A02C
#define app_reminder_75x90		 	L42C_app_icon + 0x0000ACAC

#define app_setting_40x40		 	L42C_app_icon + 0x0000E168
#define app_timer_40x40		 		L42C_app_icon + 0x0000EDE8
#define app_timer_90x90				L42C_app_icon + 0x0000FA68 
#define app_timer_reset_18x18		L42C_app_icon + 0x000139B0
#define app_timer_start_18x18		L42C_app_icon + 0x00013C38
#define app_whiteline_240x1		 	L42C_app_icon + 0x00013EC0

#define app_timer_stop_18x18	L42C_app2_icon
#define app2_activity_0_90x90	L42C_app2_icon + 0x00000288
#define app2_brightness_bar_40x40_minus		L42C_app2_icon + 0x000041D0
#define app2_brightness_bar_40x40_plus		L42C_app2_icon + 0x00004E50
#define app2_brightness_bar_108x9_0			L42C_app2_icon + 0x00005AD0
#define app2_brightness_bar_108x9_1			L42C_app2_icon + 0x00006268
#define app2_brightness_bar_108x9_2			L42C_app2_icon + 0x00006A00
#define app2_brightness_bar_108x9_3			L42C_app2_icon + 0x00007198
#define app2_brightness_bar_108x9_4			L42C_app2_icon + 0x00007930
#define app2_brightness_bar_108x9_5			L42C_app2_icon + 0x000080C8
#define app2_brightness_icon_40x40	L42C_app2_icon + 0x00008860
#define app2_brightness_icon_90x90	L42C_app2_icon + 0x000094E0
#define app2_Vibration_40x40	L42C_app2_icon + 0x0000D428


#define camara_number_1_42x68   L42C_camara_icon
#define camara_number_2_42x68   L42C_camara_icon + 0x00001650
#define camara_number_3_42x68   L42C_camara_icon + 0x00002CA0
#define camara_photos_83x65   L42C_camara_icon + 0x000042F0


#define	charing_battery0_18x12	L42C_charing_battery
#define	charing_battery1_18x12	L42C_charing_battery + 0x000001B0
#define	charing_battery2_18x12	L42C_charing_battery + 0x00000360
#define	charing_battery3_18x12	L42C_charing_battery + 0x00000510
#define	charing_battery4_18x12	L42C_charing_battery + 0x000006C0
#define	charing_green0_126x84	L42C_charing_battery + 0x00000870
#define	charing_green1_6x60		L42C_charing_battery + 0x00005B20
#define	charing_red0_126x84		L42C_charing_battery + 0x00005DF0



#define Connect_pair_90x62     		L42C_connect_icon  
#define Connect_pairfailed_16x16     L42C_connect_icon + 0x00002B98
#define Connect_pairselect_16x16     L42C_connect_icon + 0x00002D98


#define hr_char_BMP_42x16  		L42C_hr_icon
#define hr_heart_90x90  		L42C_hr_icon + 0x00000540
#define hr_heartred_90x90  		L42C_hr_icon + 0x00004488
#define hr_highheart_90x90  	L42C_hr_icon + 0x000083D0
#define hr_lowheart_90x90  		L42C_hr_icon + 0x0000C318
#define hr_minus_18x2  			L42C_hr_icon + 0x000141A8
#define hr_small_heartred_90x90  	L42C_hr_icon + 0x000141F0



#define logo_186x42    			L42C_logo_icon


#define music_cycle_all_24x18  		L42C_music_icon
#define music_cycle_random_24x18  L42C_music_icon + 0x00000360
#define music_cycle_single_24x18  L42C_music_icon + 0x000006C0
#define music_main_70x80  		  L42C_music_icon + 0x00000A20
#define music_minus_18x20         L42C_music_icon + 0x000035E0
#define music_next_18x20          L42C_music_icon + 0x000038B0
#define music_play_18x20          L42C_music_icon + 0x00003B80
#define music_pause_18x20         L42C_music_icon + 0x00003E50 
#define music_plus_18x20          L42C_music_icon + 0x00004120
#define music_point_gray_12x12    L42C_music_icon + 0x000043F0

#define music_point_white_12x12  	L42C_music_icon + 0x00004510
#define music_prev_18x20  			L42C_music_icon + 0x00004630
#define music_redround_38x38  		L42C_music_icon + 0x00004900
#define music_volume_purple_6x6  	L42C_music_icon + 0x00005448
#define music_volume_red01_6x6  	L42C_music_icon + 0x00005490 
#define music_volume_red02_6x6  	L42C_music_icon + 0x000054D8
#define music_volume_red03_6x6  	L42C_music_icon + 0x00005520
#define music_volume_red11_6x6  	L42C_music_icon + 0x00005568
#define music_volume_red12_6x6  	L42C_music_icon + 0x000055B0
#define music_volume_red13_6x6  	L42C_music_icon + 0x000055F8


#define notification_email_42x30    L42C_notification_icon
#define notification_email_90x68    L42C_notification_icon + 0x000009D8
#define notification_event_40x40    L42C_notification_icon + 0x000039A8
#define notification_event_90x90    L42C_notification_icon + 0x00004628
#define notification_icon_70x80    	L42C_notification_icon + 0x00008570 
#define notification_medicine_40x40    	L42C_notification_icon + 0x0000B130 
#define notification_message_46x40    	L42C_notification_icon + 0x0000BDB0
#define notification_message_90x90    	L42C_notification_icon + 0x0000CC10
#define notification_misscalls_40x40    L42C_notification_icon + 0x00010B58
#define notification_misscalls_90x90    L42C_notification_icon + 0x000117D8

#define notification_msgresult_90x90    		L42C_notification_icon + 0x00015720
#define notification_msgresult_failed_28x18    	L42C_notification_icon + 0x00019668
#define notification_msgresult_sussesful_28x18  L42C_notification_icon + 0x00019A10

#define notification_sleep_40x40    	L42C_notification_icon + 0x00019DB8
#define notification_social_40x40    	L42C_notification_icon + 0x0001AA38
#define notification_social_90x90    	L42C_notification_icon + 0x0001B6B8
#define notification_wakeup_40x40    	L42C_notification_icon + 0x0001F600


#define remind_char_achieved_167x24     L42C_remind_icon
#define remind_char_goal_88x24     		L42C_remind_icon + 0x00001F50
#define remind_Goal_Achieved_110x82     L42C_remind_icon + 0x00002FD0
#define remind_mealtime_57x90     		L42C_remind_icon + 0x00007648
#define remind_medal_copper_126x88     	L42C_remind_icon + 0x00009E5C
#define remind_medal_diamond_166x72     L42C_remind_icon + 0x0000F4FC
#define remind_medal_gold_126x88     	L42C_remind_icon + 0x0001525C
#define remind_medal_sillver_126x88     L42C_remind_icon + 0x0001A8FC
#define remind_medicine_69x90     		L42C_remind_icon + 0x0001FF9C
#define remind_memoryfull_70x80     	L42C_remind_icon + 0x00023020

#define remind_move_56x100     		L42C_remind_icon + 0x00025BE0
#define remind_sleep_90x90     		L42C_remind_icon + 0x000287A0
#define remind_syncefailed_80x60    L42C_remind_icon + 0x0002C6E8
#define remind_syncing_80x60     	L42C_remind_icon + 0x0002EC68
#define remind_upgrade_90x90     	L42C_remind_icon + 0x000311E8
#define remind_upgradfailed_90x90   L42C_remind_icon + 0x00035130
#define remind_wakeup_106x80     	L42C_remind_icon + 0x00039078

#define remind_activity_40x40     	L42C_remind2_icon
#define remind_inconmingcall_90x90  L42C_remind2_icon + 0x00000C80
#define remind_mealtime_40x40     	L42C_remind2_icon + 0x00004BC8
#define remind_medal_diamond_126x88 L42C_remind2_icon + 0x00005848
#define remind_medicine_40x40     	L42C_remind2_icon + 0x0000AEE8
#define remind_sleep_40x40     		L42C_remind2_icon + 0x0000BB68
#define remind_wakeup_40x40     	L42C_remind2_icon + 0x0000C7E8



#define sleep_bar_deep_2x52   	L42C_sleep_icon
#define sleep_bar_light_2x72   	L42C_sleep_icon + 0x000000D0
#define sleep_bar_wake_2x98   	L42C_sleep_icon + 0x000001F0
#define sleep_bg_blue_20x80   	L42C_sleep_icon + 0x00000378
#define sleep_detail_38x38   	L42C_sleep_icon + 0x00000FF8
#define sleep_pointawake_5x5   	L42C_sleep_icon + 0x00001B40
#define sleep_pointdeep_5x5   	L42C_sleep_icon + 0x00001B72
#define sleep_pointlight_5x5   	L42C_sleep_icon + 0x00001BA4
#define sleep_sleepblue_38x38   L42C_sleep_icon + 0x00001BD6
#define sleep_sleepwhite_38x38  L42C_sleep_icon + 0x0000271E
#define sleep_tracker_98x62   	L42C_sleep_icon + 0x00003266 


#define Char_14pxAZ_10x10 		L42C_Char_base
#define Char_14pxAZc_dot_3x10	Char_14pxAZ_10x10 + 0x00001284

#define Char_14pxazs_7x14 		L42C_Char_base + 0x0000157C
#define Char_21pxAZ_15x16 		L42C_Char_base + 0x00002DE0
#define Char_21pxazs_11x20 		L42C_Char_base + 0x00005E40
#define Char_24pxAZ_16x18 		L42C_Char_base + 0x000090B8
#define Char_24pxazs_12x24 		L42C_Char_base + 0x0000CC58
#define Char_add				L42C_Char_base + 0x00010D68
#define Char_pound_11x19		Char_add
#define Char_pound_12x16		Char_add + 0x000001A2	


#define number12px_gray_7x9     	L42C_typeface_base
#define number24px_14x18     		L42C_typeface_base + 0x00000522
#define number36px_18x26     		L42C_typeface_base + 0x000018D2
#define remindnumber14px_9x10     	L42C_typeface_base + 0x00004172

#define sleepblue_month     		L42C_typeface_base + 0x0000487A
#define sleepblue_month_1_33x20		sleepblue_month
#define sleepblue_month_2_33x20		sleepblue_month + 0x00000528
#define sleepblue_month_3_33x20		sleepblue_month + 0x00000A28
#define sleepblue_month_4_33x20		sleepblue_month + 0x00000FA0
#define sleepblue_month_5_33x20		sleepblue_month + 0x000014A0
#define sleepblue_month_6_33x20		sleepblue_month + 0x00001A90
#define sleepblue_month_7_33x20		sleepblue_month + 0x00002170
#define sleepblue_month_8_33x20		sleepblue_month + 0x00002738
#define sleepblue_month_9_33x20		sleepblue_month + 0x00002CB0
#define sleepblue_month_10_33x20		sleepblue_month + 0x00003318
#define sleepblue_month_11_33x20		sleepblue_month + 0x000037C8
#define sleepblue_month_12_33x20		sleepblue_month + 0x00003D40


#define sleepblue21px_number_12x16  L42C_typeface_base + 0x00008AE2
#define sleepblue21px_number_comma_4x16 sleepblue21px_number_12x16 + 0x00001080

#define sleepblue36px_number_18x26  L42C_typeface_base + 0x00009BE2
#define sleepblue36px_number_h_13x26	sleepblue36px_number_18x26 + 0x00002490
#define sleepblue36px_number_m_13x26	sleepblue36px_number_18x26 + 0x00002734

#define socialnumber21px_12x16     	L42C_typeface_base + 0x0000C6BE


#define timenumber14px_8x11     	L42C_typeface_base + 0x0000D5BE
#define timernumber32px_18x24     	L42C_typeface_base + 0x0000DCF6


#define activity_barH_b_240x10 		L42C_bar_base
#define music_bar			 		L42C_bar_base + 0x00000E10

#define activity_barH_b_gray_10x10  activity_barH_b_240x10
#define activity_barH_b1_gray_10x10  activity_barH_b_240x10 + 0x000000C8
#define activity_barH_blue1_10x10  activity_barH_b_240x10 + 0x00000190
#define activity_barH_blue2_10x10  activity_barH_b_240x10 + 0x00000258
#define activity_barH_blue3_10x10  activity_barH_b_240x10 + 0x00000320
#define activity_barH_blue4_10x10  activity_barH_b_240x10 + 0x000003E8
#define activity_barH_green1_10x10  activity_barH_b_240x10 + 0x000004B0
#define activity_barH_green2_10x10  activity_barH_b_240x10 + 0x00000578
#define activity_barH_green3_10x10  activity_barH_b_240x10 + 0x00000640
#define activity_barH_green4_10x10  activity_barH_b_240x10 + 0x00000708
#define activity_barH_red1_10x10  activity_barH_b_240x10 + 0x000007D0
#define activity_barH_red2_10x10  activity_barH_b_240x10 + 0x00000898
#define activity_barH_red3_10x10  activity_barH_b_240x10 + 0x00000960
#define activity_barH_red4_10x10  activity_barH_b_240x10 + 0x00000A28
#define activity_barH_yellow1_10x10  activity_barH_b_240x10 + 0x00000AF0
#define activity_barH_yellow2_10x10  activity_barH_b_240x10 + 0x00000BB8
#define activity_barH_yellow3_10x10  activity_barH_b_240x10 + 0x00000C80
#define activity_barH_yellow4_10x10  activity_barH_b_240x10 + 0x00000D48

#define music_bar_b_4x2			music_bar
#define music_bar_el_8x16		music_bar + 0x00000010
#define music_bar_eline_10x1	music_bar + 0x00000110
#define music_bar_er_8x16		music_bar + 0x00000124
#define music_bar_t_4x2			music_bar + 0x00000224
#define music_bar_tl_4x2		music_bar + 0x00000234
#define music_bar_tr_4x2		music_bar + 0x00000244



#define time_distance_22x20 		timeblack_base
#define time_redheart0_22x20 		timeblack_base + 0x00000370
#define time_redheart1_22x20 		timeblack_base + 0x000006E0
#define time_sporttimer_22x20 		timeblack_base + 0x00000A50
#define time_step_22x20 			timeblack_base + 0x00000DC0
#define time_calories_22x20 	timeblack_base + 0x00001130

#define timeblack_date_9x15 	timeblack_base + 0x000014A0

#define timeblack_linedotted_158x2 	timeblack_base + 0x00001F2C
#define timeblack_linereal_158x2 	timeblack_base + 0x000021A4

#define timeblack_month1_33x18 	timeblack_base + 0x0000241C 
#define timeblack_month2_33x18 	timeblack_base + 0x000028C0
#define timeblack_month3_35x18 	timeblack_base + 0x00002D64
#define timeblack_month4_31x18 	timeblack_base + 0x00003250
#define timeblack_month5_38x18 	timeblack_base + 0x000036AC 
#define timeblack_month6_46x18 	timeblack_base + 0x00003C04
#define timeblack_month7_38x18 	timeblack_base + 0x0000427C
#define timeblack_month8_35x18 	timeblack_base + 0x000047D4
#define timeblack_month9_42x18 	timeblack_base + 0x00004CC0
#define timeblack_month10_31x18 	timeblack_base + 0x000052A8
#define timeblack_month11_35x18 	timeblack_base + 0x00005704
#define timeblack_month12_34x18 	timeblack_base + 0x00005BF0

#define timeblack_timeblue_35x55	timeblack_base + 0x000060B8
#define timeblack_timewhite_35x55	timeblack_base + 0x0000F71C
#define timeblack_timer_AM_20x55	timeblack_base + 0x00018D80
#define timeblack_timer_PM_20x55	timeblack_base + 0x00019618



#define timestar_backgrond_240x240 	timestar_base 
#define timestar_date_8x10 			timestar_base + 0x0001C200

#define timestar_month_1_22x14		timestar_base + 0x0001C840
#define timestar_month_2_22x14		timestar_base + 0x0001CAA8
#define timestar_month_3_24x14		timestar_base + 0x0001CD10
#define timestar_month_4_22x14		timestar_base + 0x0001CFB0
#define timestar_month_5_26x14		timestar_base + 0x0001D218
#define timestar_month_6_31x14		timestar_base + 0x0001D4F0
#define timestar_month_7_26x14		timestar_base + 0x0001D854
#define timestar_month_8_24x14		timestar_base + 0x0001DB2C
#define timestar_month_9_28x14		timestar_base + 0x0001DDCC
#define timestar_month_10_22x14		timestar_base + 0x0001E0DC
#define timestar_month_11_24x14		timestar_base + 0x0001E344
#define timestar_month_12_24x14		timestar_base + 0x0001E5E4

#define timestar_time_32x46			timestar_base + 0x0001E884
#define timestar_time_colon_15x46   timestar_base + 0x00025B84
#define timestar_week_1_27x11		timestar_base + 0x000260E8
#define timestar_week_2_23x11		timestar_base + 0x0002633A
#define timestar_week_3_27x11		timestar_base + 0x00026534
#define timestar_week_4_23x11		timestar_base + 0x00026786
#define timestar_week_5_14x11		timestar_base + 0x00026980
#define timestar_week_6_21x11		timestar_base + 0x00026AB4
#define timestar_week_7_23x11		timestar_base + 0x00026C82
#define timestar_battery_0_28x16		timestar_base + 0x00026E7C
#define timestar_battery_1_28x16		timestar_base + 0x000271FC
#define timestar_battery_2_28x16		timestar_base + 0x0002757C
#define timestar_battery_3_28x16		timestar_base + 0x000278FC
#define timestar_battery_4_28x16		timestar_base + 0x00027C7C
#define timestar_timer_AM_20x46		timestar_base + 0x00027FFC
#define timestar_timer_PM_20x46		timestar_base + 0x0002872C


#define timesunup_backg4				timesunup_base
#define timesunup_battery_0_28x16		timesunup_base + 0x0001C200
#define timesunup_battery_1_28x16		timesunup_base + 0x0001C580
#define timesunup_battery_2_28x16		timesunup_base + 0x0001C900
#define timesunup_battery_3_28x16		timesunup_base + 0x0001CC80
#define timesunup_battery_4_28x16		timesunup_base + 0x0001D000

#define timesunup_date_9x15		timesunup_base + 0x0001D380

#define timesunup_month_1_33x18		timesunup_base + 0x0001DE0C
#define timesunup_month_2_33x18		timesunup_base + 0x0001E2B0
#define timesunup_month_3_35x18		timesunup_base + 0x0001E754
#define timesunup_month_4_31x18		timesunup_base + 0x0001EC40
#define timesunup_month_5_38x18		timesunup_base + 0x0001F09C
#define timesunup_month_6_46x18		timesunup_base + 0x0001F5F4
#define timesunup_month_7_38x18		timesunup_base + 0x0001FC6C
#define timesunup_month_8_35x18		timesunup_base + 0x000201C4
#define timesunup_month_9_42x18		timesunup_base + 0x000206B0
#define timesunup_month_10_31x18		timesunup_base + 0x00020C98
#define timesunup_month_11_35x18		timesunup_base + 0x000210F4
#define timesunup_month_12_31x18		timesunup_base + 0x000215E0

#define timesunup_number_34x50		timesunup_base + 0x00021AA8
#define timesunup_number_colon_17x50		timesunup_base + 0x00029F78

#define timesunup_week_1_39x15		timesunup_base + 0x0002A61C
#define timesunup_week_2_34x15		timesunup_base + 0x0002AAAE
#define timesunup_week_3_40x15		timesunup_base + 0x0002AEAA
#define timesunup_week_4_35x15		timesunup_base + 0x0002B35A
#define timesunup_week_5_22x15		timesunup_base + 0x0002B774
#define timesunup_week_6_31x15		timesunup_base + 0x0002BA08
#define timesunup_week_7_35x15		timesunup_base + 0x0002BDAA
#define timesunup_timer_AM_20x50		timesunup_base + 0x0002C1C4
#define timesunup_timer_PM_20x50		timesunup_base + 0x0002C994


//#define ancs_backgroud_1_up_218x44 		L42C_ANCS_BG_icon
//#define ancs_backgroud_2_center_218x24 	L42C_ANCS_BG_icon + 0x00004AF0
//#define ancs_backgroud_3_down_218x20 	L42C_ANCS_BG_icon + 0x000073D0

#define ancs_backgroud_1_up_218x46 		L42C_ANCS_BG1_icon
#define ancs_backgroud_2_center_218x26 	L42C_ANCS_BG1_icon + 0x00004E58
#define ancs_backgroud_31_down_218x20 	L42C_ANCS_BG1_icon + 0x00007AA0


#define bp_funnel_60x80		L42D_bp_icon_base
#define bp_icon_0_64x95		L42D_bp_icon_base + 0x00002580
#define bp_icon_1_64x95		L42D_bp_icon_base + 0x00005500
#define bp_select_fail_32x32		L42D_bp_icon_base + 0x00008480
#define bp_select_sucess_32x32		L42D_bp_icon_base + 0x00008C80



#define bt2_bluetooth_36x68			L42D_bt2_bicon_base
#define bt2_dots_42x2				L42D_bt2_bicon_base + 0x00001320
#define bt2_phone_34x60				L42D_bt2_bicon_base + 0x000013C8
#define bt2_result_failed_46x44		L42D_bt2_bicon_base + 0x000023B8
#define bt2_result_succes_46x44		L42D_bt2_bicon_base + 0x00003388
#define bt2_watch_22x54				L42D_bt2_bicon_base + 0x00004358


#define dail_answer_72x72			L42D_dail_icon_base
#define dail_callended_84x34		L42D_dail_icon_base + 0x00002880
#define dail_hangup_72x72			L42D_dail_icon_base + 0x00003ED0
#define dail_keyboard_56x56			L42D_dail_icon_base + 0x00006750
#define dail_mute_gray_72x72		L42D_dail_icon_base + 0x00007FD0
#define dail_mute_yellow_56x56		L42D_dail_icon_base + 0x0000A850
#define dail_sound_gray_56x56		L42D_dail_icon_base + 0x0000D0D0
#define dail_sound_white_56x56		L42D_dail_icon_base + 0x0000E950


#define add_camera					L42_add_icon
#define add_findphone				L42_add_icon + 0x000026D0
#define add_music					L42_add_icon + 0x000075F8
#define add_notification			L42_add_icon + 0x0000DB08
#define add_recordworkouts			L42_add_icon + 0x00017E48
#define add_remind					L42_add_icon + 0x0002B7A0
#define sleep_action				L42_add_icon + 0x00035748
#define char_percent_22x24			L42_add_icon + 0x000369D2
#define vibration_40x40				L42_add_icon + 0x00036DF2
#define timesunup_singlequote_3x15	L42_add_icon + 0x00037A72
#define timemain_bigbatnumber		L42_add_icon + 0x00037ACC

#define camera_icon_48x36			add_camera
#define camera_warming_60x54		add_camera + 0x00000D80

#define findphone_phone_40x40			add_findphone
#define findphone_phone_48x88			add_findphone + 0x00000C80
#define findphone_position_30x38		add_findphone + 0x00002D80
#define findphone_ware_left_18x88		add_findphone + 0x00003668
#define findphone_ware_right_18x88		add_findphone + 0x000042C8



#define music_icon_42x50			add_music
#define music_pause_44x44			add_music + 0x00001068
#define music_play_44x44		add_music + 0x00001F88
#define music_round_82x82		add_music + 0x00002EA8
#define music_vol_back_6x20		add_music + 0x00006330
#define music_vol_on_6x20		add_music + 0x00006420

#define notification_40x40_facebook_L42			add_notification
#define notification_40x40_instagram_L42		add_notification + 0x00000C80
#define notification_40x40_line_L42				add_notification + 0x00001900
#define notification_40x40_messenger_L42		add_notification + 0x00002580
#define notification_40x40_qqwechat_L42			add_notification + 0x00003200
#define notification_40x40_snapchat_L42			add_notification + 0x00003E80
#define notification_40x40_twitter_L42			add_notification + 0x00004B00
#define notification_40x40_whatsapp_L42			add_notification + 0x00005780
#define notifications_menu_clearall_184x44		add_notification + 0x00006400

#define recordworkouts_40x40_calories		add_recordworkouts
#define recordworkouts_40x40_distance		add_recordworkouts + 0x00000C80
#define recordworkouts_40x40_hr				add_recordworkouts + 0x00001900
#define recordworkouts_40x40_steps			add_recordworkouts + 0x00002580
#define recordworkouts_40x40_timer			add_recordworkouts + 0x00003200
#define recordworkouts_90x90_main			add_recordworkouts + 0x00003E80
#define recordworkouts_92x44_go				add_recordworkouts + 0x00007DC8
#define recordworkouts_char_results_96x24	add_recordworkouts + 0x00009D68
#define recordworkouts_donemenu_184x44		add_recordworkouts + 0x0000AF68
#define recordworkouts_state_flag_46x24		add_recordworkouts + 0x0000EEA8
#define recordworkouts_state_pause_24x32	add_recordworkouts + 0x0000F748
#define recordworkouts_state_play_62x62		add_recordworkouts + 0x0000FD48
#define recordworkouts_state_round_62x62	add_recordworkouts + 0x00011B50

#define remind_40x40_breaktime			add_remind
#define remind_40x40_eat				add_remind + 0x00000C80
#define remind_40x40_pill				add_remind + 0x00001900
#define remind_90x90_breaktime			add_remind + 0x00002580
#define remind_incomingcalls_66x88		add_remind + 0x000064C8
#define remind_wakeup_48x36				add_remind + 0x00009228

#define sleep_round_38x38			sleep_action
#define sleep_detail_20x20				sleep_action + 0x00000B48
#define sleep_play_18x20				sleep_action + 0x00000E68
#define sleep_stop_13x13			sleep_action + 0x00001138

#define timemain_bat_28x16			timemain_bigbatnumber
#define timemain_number_11x16		timemain_bigbatnumber + 0x00001180
#define timemain_number_4x16_colon	timemain_number_11x16 + 0x00000DC0


#define time1_red_big_8x8		time1_dots_base
#define time1_red_small_4x4		time1_dots_base + 0x00000080
#define time1_grey_big_8x8		time1_dots_base + 0x000000A0
#define time1_grey_small_4x4	time1_dots_base + 0x00000120
#define timeblack_timewhite_colon_12x55		time1_dots_base + 0x00000140

#define timeblue_bat_28x16		timeblue_base
#define timeblue_date_9x15		timeblue_base + 0x00001180 
#define timeblue_icon			timeblue_base + 0x00001C0C
#define timeblue_month			timeblue_base + 0x0001E758
#define timeblue_number_11x18	timeblue_base + 0x00021E12
#define timeblue_number_5x18_dot timeblue_number_11x18 + 0x00000F78
#define timeblue_time_30x45		timeblue_base + 0x00022E3E
#define timeblue_time_13x45_colon timeblue_time_30x45 + 0x00006978	
#define timeblue_week			timeblue_base + 0x00029C48



#define timeblue_char_BPM_37x18 	timeblue_base + 0x0002C7BC	
#define timeblue_icon_hr_38x38_0	timeblue_base + 0x0002CCF0



#define	timeblue_back_212x212			timeblue_icon
#define	timeblue_char_BMP_37x18			timeblue_icon + 0x00015F20
#define	timeblue_char_calories_64x18	timeblue_icon + 0x00016454
#define	timeblue_char_Km_25x18			timeblue_icon + 0x00016D54
#define	timeblue_char_miles_42x18		timeblue_icon + 0x000170D8
#define	timeblue_char_minutes_64x18		timeblue_icon + 0x000176C0
#define	timeblue_char_steps_44x18		timeblue_icon + 0x00017FC0
#define	timeblue_icon_calories_38x38	timeblue_icon + 0x000185F0
#define	timeblue_icon_distance_38x38	timeblue_icon + 0x00019138
#define	timeblue_icon_hr_38x38			timeblue_icon + 0x00019C80
#define	timeblue_icon_line_1x38			timeblue_icon + 0x0001A7C8
#define	timeblue_icon_step_38x38		timeblue_icon + 0x0001A814
#define	timeblue_icon_timer_38x38		timeblue_icon + 0x0001B35C
#define	timeblue_timeAM_18x45			timeblue_icon + 0x0001BEA4
#define	timeblue_timePM_18x45			timeblue_icon + 0x0001C4F8


#define timeblue_month_1_36x15 timeblue_month
#define timeblue_month_2_31x15 timeblue_month + 0x00000438
#define timeblue_month_3_40x15 timeblue_month + 0x000007DA
#define timeblue_month_4_36x15 timeblue_month + 0x00000C8A
#define timeblue_month_5_40x15 timeblue_month + 0x000010C2
#define timeblue_month_6_46x15 timeblue_month + 0x00001572
#define timeblue_month_7_46x15 timeblue_month + 0x00001AD6
#define timeblue_month_8_38x15 timeblue_month + 0x0000203A
#define timeblue_month_9_46x15 timeblue_month + 0x000024AE
#define timeblue_month_10_36x15 timeblue_month + 0x00002A12
#define timeblue_month_11_38x15 timeblue_month + 0x00002E4A
#define timeblue_month_12_34x15 timeblue_month + 0x000032BE


#define timeblue_week_1_50x18 timeblue_week
#define timeblue_week_2_44x18 timeblue_week + 0x00000708
#define timeblue_week_3_49x18 timeblue_week + 0x00000D38
#define timeblue_week_4_46x18 timeblue_week + 0x0000141C
#define timeblue_week_5_32x18 timeblue_week + 0x00001A94
#define timeblue_week_6_44x18 timeblue_week + 0x00001F14
#define timeblue_week_7_44x18 timeblue_week + 0x00002544
#define timeblue_week_U_15x18		timeblue_week_2_44x18 + 0x0000021C



#define timesierra_back_240x240 	timesierra_base
#define timesierra_bat_28x16 		timesierra_base + 0x0001C200
#define timesierra_date_9x15 		timesierra_base + 0x0001D380
#define timesierra_date_3x15_comma	timesierra_date_9x15 + 0x00000A8C
#define timesierra_month 			timesierra_base + 0x0001DE66
#define timesierra_time_32x50 		timesierra_base + 0x00020DC6

#define timesierra_month_1_28x16 timesierra_month
#define timesierra_month_2_30x16 timesierra_month + 0x00000380
#define timesierra_month_3_30x16 timesierra_month + 0x00000740
#define timesierra_month_4_28x16 timesierra_month + 0x00000B00
#define timesierra_month_5_33x16 timesierra_month + 0x00000E80 
#define timesierra_month_6_38x16 timesierra_month + 0x000012A0
#define timesierra_month_7_32x16 timesierra_month + 0x00001760
#define timesierra_month_8_31x16 timesierra_month + 0x00001B60
#define timesierra_month_9_36x16 timesierra_month + 0x00001F40
#define timesierra_month_10_30x16 timesierra_month + 0x000023C0
#define timesierra_month_11_31x16 timesierra_month + 0x00002780
#define timesierra_month_12_32x16 timesierra_month + 0x00002B60


#endif
