/*------------------------------------------------------------------------\
|  weather.c :  Weather Module                        www.middle-earth.us |
|  Copyright (C) 2004, Shadows of Isildur: Traithe                        |
|  Subsequently ravaged by Kithrater in 2010 for AtonementRPI             |
|  Derived under license from DIKU GAMMA (0.0).                           |
\------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "structs.h"
#include "protos.h"
#include "utils.h"
#include "utility.h"

Weather weather_info[100];

int global_moon_light = 0;
int moon_light[100];
int sun_light = 1;
int desc_weather[100];
AFFECTED_TYPE *world_affects = NULL;
const int sunrise[] = { 7, 6, 6, 6, 6, 5, 4, 4, 5, 5, 6, 6 };
const int sunset[] = { 18, 18, 19, 19, 20, 21, 22, 22, 21, 20, 19, 18 };

//test
const char *earth_phase[] =
{
    "Full Earth",
    "Gibbous Waning",
    "Three Quarter",
    "Crescent Waning",
    "New Earth",
    "Crescent Waxing",
    "First Quarter",
    "Gibbous Waxing"
};

const char *abrev_earth_phase[] =
{
    "FEa",
    "GWn",
    "TQu",
    "CWn",
    "NEa",
    "CWx",
    "FQu",
    "GWx",
	"",
};

const char *sun_phase[] =
{
    "night time",
    "before dawn",
    "dawn",
    "day time",
    "high sun",
    "dusk",
    "evening"
};

const char *temp_phrase[] =
{
	"cool",
	"dangerously searing",
	"painfully blazing",
	"blistering",
	"sweltering",
	"hot",
	"temperate",
	"cool",
	"chill",
	"cold",
	"very cold",
	"frigid",
	"freezing",
	"numbingly frigid",
	"painfully freezing",
	"dangerously freezing"
};

const char *wind_temp_phrase[] =
{
    "cool",
    "searing",
    "scorching",
    "sweltering",
    "hot",
    "warm",
    "cool",
    "chill",
    "cold",
    "frigid",
    "freezing",
    "artic"
};

// How warm the area is is dependant upon the phase of the Sun.
// 0 = set,
// 1 = pre-dawn
// 2 = dawn
// 3 = risen
// 4 = midday
// 5 = dusk
// 6 = evening


// const int seasonal_temp[6][12] =
// {
//    {90, 85, 70, 50, 35, 25, 20, 25, 35, 50, 70, 85},	/* Wilderness Baseline */
//    {90, 85, 70, 60, 45, 35, 30, 35, 45, 60, 70, 85},	/* Urban Baseline */
//    {80, 75, 60, 50, 35, 25, 20, 25, 35, 50, 60, 75},	/* Desert Baseline */
//    {70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70},	/* Underground Baseline */
//    {60, 55, 40, 30, 25, 15, 10, 15, 25, 30, 40, 55},	/* Crater Baseline */
//    { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}	/* Nolight Baseline */
// };

const int seasonal_temp[7][12] = {
	{40, 47, 55, 63, 71, 79, 85, 79, 71, 63, 55, 47},	/* Temperate baseline */

	{22, 29, 37, 45, 53, 61, 67, 61, 53, 45, 37, 29},	/* Cool */
	{15, 22, 28, 38, 46, 50, 55, 50, 46, 38, 28, 22},	/* Cold */
	{7, 14, 18, 22, 25, 27, 30, 27, 25, 22, 18, 14},	/* Arctic */

	{55, 57, 60, 65, 73, 81, 89, 81, 73, 65, 60, 53},	/* Hot */
	{60, 62, 65, 70, 78, 86, 94, 86, 78, 70, 65, 58},	/* Hot */
	{75, 77, 80, 85, 93, 101, 109, 101, 93, 85, 80, 73}	/* Desert */
};
void
initialize_weather_zones (void)
{
    // Dorthonion and its surrounding lands are -cold-.
    //zone_table[10].weather_type = WEATHER_COLD;
};



bool Weather::weather_unification (int zone)
{
    bool zone_updated = false;

    // Begin Japheth's "Weather zone unification" changes
    if (zone >= 1 && zone <= 2)
    {
        weather_info[zone] = weather_info[10];
        zone_updated = true;
    }

    return zone_updated;
}

/*
int calcTemp(int zone)
{
    float temp_base = 20;
    float temp_max = 100;
    // Our temperature.
    float i = 0;
    float mid_point = 336 + 84;
    int true_hour = time_info.hour + (84 * time_info.day);


    // We have 672 hours in our cycle,
    // Sun rises at 192,
    // Sun is at peak at 336,
    // Sun sets at 588,
    // So, from 0 to 336+84 (heat takes longer to dissipate), we grow hotter
    // from 336 + 84 to 671, we grow colder.
    // So, our actual temperature for growing hotter is going to be temp_max * current_hour / actual_hour
    // For growing colder, it's going to be temp_max - (temp_max * (336+84) / actual_hour);
    // TODO: have temp_base and temp_max be set by zone type.

    if (time_info.season == SEASON_FAR)
    {
        temp_base -= 10;
        temp_max -= 10;
    }
    else
    {
        temp_base += 10;
        temp_max += 10;
    }

    if (true_hour < mid_point)
    {
        i = temp_max * (true_hour / mid_point) + temp_base;
    }
    else
    {
        i = temp_max - (temp_max * ((true_hour - mid_point) / mid_point));
    }

    // Some sanity checks.

    //if (zone == 10)
    //{
    //    std::string togods = "i: " + MAKE_STRING(i) + ", true_hour: " + MAKE_STRING(true_hour) + ", time_info.day: " + MAKE_STRING(time_info.day);
    //    send_to_gods(togods.c_str());
    //}

    i = MIN(i, temp_max);
    i = MAX(i, temp_base);


    //if (zone == 10)
    //{
    //    std::string togods = MAKE_STRING((int) i);
    //    send_to_gods(togods.c_str());
    //}

    return (int) i;

}
*/

void
weather (int moon_setting, int moon_rise, int moon_set)
{
    // Temperatues go between 10 to 90 degrees fahrenheit: near ups these by ten, far drops these by ten.
    int temp_base = 20;
    int temp_max = 100;
    int last_temp = 0, last_clouds = 0, last_state = 0;
    int roll = 0, chance_of_rain = 0, last_fog = 0, i = 0;
    char buf[MAX_STRING_LENGTH];
    char storm[MAX_STRING_LENGTH];

    for (i = 0; i <= 99; i++)
    {
        if (Weather::weather_unification (i))
            continue;
        chance_of_rain = 0; 
        
        last_temp = weather_info[i].temperature;
        last_clouds = weather_info[i].clouds;
        last_state = weather_info[i].state;
        last_fog = weather_info[i].fog;

        weather_info[i].temperature = seasonal_temp[zone_table[i].weather_type][time_info.month];
        
      	if (time_info.hour == sunrise[time_info.month])
			weather_info[i].trend = (weather_info[i].trend * 2 + number (0, 15)) / 3;

      	weather_info[i].temperature += weather_info[i].trend;        

        if ((weather_info[i].wind_speed < STORMY) && (weather_info[i].wind_speed > CALM))
        {
            if (weather_info[i].wind_dir == WEST_WIND)
            {
                if (!number (0, 47))
                {
                    send_outside_zone ("The westerly winds wane and colder air descends out of the north.\n\r", i);
                    weather_info[i].wind_dir = NORTH_WIND;
                }
            }
            else
            {
                if (!number (0, 23))
                {
                    send_outside_zone ("The warmer prevailing winds rise up out of the west, driving off the chill northerlies.\n\r", i);
                    weather_info[i].wind_dir = WEST_WIND;
                }
            }
        }

      if (!number (0, 15) && weather_info[i].wind_speed)
	{
	  roll = number (0, 99);
	  roll += weather_info[i].temperature / 3;

	  if ((time_info.season == SPRING) || (time_info.season == AUTUMN))
	    {
	      if (roll < 20)
		{
		  if (last_clouds == OVERCAST)
		    weather_info[i].clouds = HEAVY_CLOUDS;
		  else
		    weather_info[i].clouds = CLEAR_SKY;
		}
	      else if (roll < 45)
		weather_info[i].clouds = LIGHT_CLOUDS;
	      else if (roll < 80)
		weather_info[i].clouds = HEAVY_CLOUDS;
	      else
		{
		  if (last_clouds == CLEAR_SKY)
		    weather_info[i].clouds = LIGHT_CLOUDS;
		  else
		    weather_info[i].clouds = OVERCAST;
		}
	    }
	  else if (time_info.season == SUMMER)
	    {
	      if (roll < 50)
		{
		  if (last_clouds == OVERCAST)
		    weather_info[i].clouds = HEAVY_CLOUDS;
		  else
		    weather_info[i].clouds = CLEAR_SKY;
		}
	      else if (roll < 80)
		weather_info[i].clouds = LIGHT_CLOUDS;
	      else if (roll < 90)
		weather_info[i].clouds = HEAVY_CLOUDS;
	      else
		{
		  if (last_clouds == CLEAR_SKY)
		    weather_info[i].clouds = LIGHT_CLOUDS;
		  else
		    weather_info[i].clouds = OVERCAST;
		}
	    }
	  else
	    {
	      if (roll < 10)
		{
		  if (last_clouds == OVERCAST)
		    weather_info[i].clouds = HEAVY_CLOUDS;
		  else
		    weather_info[i].clouds = CLEAR_SKY;
		}
	      else if (roll < 25)
		weather_info[i].clouds = LIGHT_CLOUDS;
	      else if (roll < 75)
		weather_info[i].clouds = HEAVY_CLOUDS;
	      else
		{
		  if (last_clouds == CLEAR_SKY)
		    weather_info[i].clouds = LIGHT_CLOUDS;
		  else
		    weather_info[i].clouds = OVERCAST;
		}
	    }
	}

      if (weather_info[i].fog < THICK_FOG)
	{
	  if ((weather_info[i].clouds == CLEAR_SKY)
	      && (weather_info[i].clouds != last_clouds))
	    {
	      if (weather_info[i].wind_dir == WEST_WIND)
		send_outside_zone
		  ("The clouds are born away upon the prevailing winds and clear skies open up above.\n",
		   i);
	      if (weather_info[i].wind_dir == NORTH_WIND)
		send_outside_zone
		  ("The northern winds carry away the cloud cover, leaving the sky clear.\n\r",
		   i);
	    }

	  if ((weather_info[i].clouds == LIGHT_CLOUDS)
	      && (weather_info[i].clouds != last_clouds))
	    {
	      if (weather_info[i].wind_dir == WEST_WIND)
		{
		  if (last_clouds > weather_info[i].clouds)
		    send_outside_zone
		      ("The cloud cover begins to clear, carried eastward upon the prevailing winds.\n\r",
		       i);
		  if (last_clouds < weather_info[i].clouds)
		    send_outside_zone
		      ("Wisplike clouds drift out of the west upon the prevailing winds.\n\r",
		       i);
		}

	      if (weather_info[i].wind_dir == NORTH_WIND)
		{
		  if (last_clouds > weather_info[i].clouds)
		    send_outside_zone
		      ("The cloud cover begins to clear, carried southward upon the chill northern winds.\n\r",
		       i);
		  if (last_clouds < weather_info[i].clouds)
		    send_outside_zone
		      ("Threadlike clouds begin to drift overhead upon the chill northern winds.\n\r",
		       i);
		}
	    }

	  if ((weather_info[i].clouds == HEAVY_CLOUDS)
	      && (weather_info[i].clouds != last_clouds))
	    {
	      if (weather_info[i].wind_dir == WEST_WIND)
		{
		  if (last_clouds < weather_info[i].clouds)
		    send_outside_zone
		      ("A host of clouds marches out of the west upon the prevailing winds.\n\r",
		       i);
		  if (last_clouds > weather_info[i].clouds)
		    send_outside_zone
		      ("Small patches of sky open up as the storm clouds drift eastward.\n\r",
		       i);
		}

	      if (weather_info[i].wind_dir == NORTH_WIND)
		{
		  if (last_clouds > weather_info[i].clouds)
		    send_outside_zone
		      ("Small patches of sky peek through the cloud cover as the storm clouds move southward.\n\r",
		       i);
		  if (last_clouds < weather_info[i].clouds)
		    send_outside_zone
		      ("The chill northerly winds bring heavy clouds in their wake.\n\r",
		       i);
		}
	    }

	  if ((weather_info[i].clouds == OVERCAST)
	      && (weather_info[i].clouds != last_clouds))
	    {
	      if (weather_info[i].wind_dir == WEST_WIND)
		{
		  if (sun_light == 1)
		    send_outside_zone
		      ("The prevailing winds bring a blanket of thick storm clouds to obscure Anor.\n\r",
		       i);
		  else
		    send_outside_zone
		      ("The prevailing winds bring a blanket of thick storm clouds into the sky.\n\r",
		       i);
		}

	      if (weather_info[i].wind_dir == NORTH_WIND)
		{
		  if (sun_light == 1)
		    send_outside_zone
		      ("A thick veil of storm clouds sweeps out of the north, plunging the land into twilight.\n\r",
		       i);
		  else
		    send_outside_zone
		      ("A thick veil of storm clouds sweeps out of the north.\n\r",
		       i);
		}
	    }
	}

      if (weather_info[i].clouds != last_clouds)
	{			/*   Is the new front a rain front?   */

	  if (time_info.season == SPRING)	/*   Spring rains   */
	    chance_of_rain = 20;

	  if (weather_info[i].clouds == CLEAR_SKY)	/*   More clouds = Higher chance of rain   */
	    chance_of_rain = 0;
	  else
	    chance_of_rain += (weather_info[i].clouds * 15);

	  if (number (0, 99) < chance_of_rain)
	    {
	      weather_info[i].state = CHANCE_RAIN;
	    }
	  else if ((last_state > CHANCE_RAIN) && (last_state < LIGHT_SNOW))
	    {
	      weather_info[i].state = NO_RAIN;
	      send_outside_zone ("The rain passes.\n\r", i);
	    }
	  else if (last_state > HEAVY_RAIN)
	    {
	      weather_info[i].state = NO_RAIN;
	      send_outside ("It stops snowing.\n\r");
	    }
	}

      /*   Lightning is more common the closer you get to midsummer. I wanted it to be more common   */
      /*   with higher temperatures, but we haven't determined temp and we need to know now.   */

      if ((weather_info[i].clouds > LIGHT_CLOUDS)
	  && (weather_info[i].state > NO_RAIN))
	{
	  if (number (35, 350) <
	      seasonal_temp[zone_table[i].weather_type][time_info.month])
	    {
	      if (number (1, 10) && number (1, 3) < weather_info[i].clouds)
		{
		  weather_info[i].lightning = 1;
		  send_outside_zone
		    ("Lightning flashes across the heavens.\n\r", i);
		}
	      else
		weather_info[i].lightning = 0;
	    }
	}

      if (!number (0, 4))
	{
	  roll = number (-1, 1);
	  switch (roll)
	    {
	    case -1:
	      if (weather_info[i].wind_speed == CALM)
		break;
	      if (weather_info[i].wind_speed == BREEZE && number (0, 1))
		break;
	      weather_info[i].wind_speed -= 1;
	      if (weather_info[i].wind_speed == CALM)
		send_outside_zone ("The winds die and the air stills.\n\r",
				   i);
	      if (weather_info[i].wind_speed == BREEZE)
		send_outside_zone ("The wind dies down to a mild breeze.\n\r",
				   i);
	      if (weather_info[i].wind_speed == WINDY)
		send_outside_zone
		  ("The gale winds die down to a steady current.\n\r", i);
	      if (weather_info[i].wind_speed == GALE)
		send_outside_zone
		  ("The stormy winds slow to a steady gale.\n\r", i);
	      break;

	    case 1:
	      sprintf (storm, "wind storm");
	      if (weather_info[i].state > CHANCE_RAIN)
		sprintf (storm, "rain storm");
	      if (weather_info[i].lightning)
		sprintf (storm, "thunder storm");
	      if (weather_info[i].state > HEAVY_RAIN)
		sprintf (storm, "blizzard");
	      if (weather_info[i].wind_speed == STORMY)
		{
		  send_outside_zone
		    ("The storm winds slow, leaving a steady gale in their wake.\n\r",
		     i);
		  weather_info[i].wind_speed -= 1;
		  break;
		}
	      if (weather_info[i].wind_speed == CALM)
		send_outside_zone ("A capricious breeze picks up.\n\r", i);
	      if (weather_info[i].wind_speed == BREEZE)
		{
		  if (number (0, 1))
		    break;
		  send_outside_zone
		    ("The breeze strengthens into a steady wind.\n\r", i);
		}
	      if (weather_info[i].wind_speed == WINDY)
		{
		  if (!number (0, 3))
		    break;
		  if (weather_info[i].state < LIGHT_RAIN)
		    send_outside_zone
		      ("The winds grow fierce, building into a strong gale.\n\r",
		       i);
		  else
		    {
		      if (weather_info[i].state > HEAVY_RAIN)
			sprintf (storm, "snow storm");
		      sprintf (buf,
			       "The winds grow fierce, building into a mild %s.\n\r",
			       storm);
		      send_outside_zone (buf, i);
		    }
		}
	      if (weather_info[i].wind_speed == GALE)
		{
		  if (!number (0, 5))
		    break;
		  sprintf (buf,
			   "The winds begin to rage, and a fierce %s is born.\n\r",
			   storm);
		  send_outside_zone (buf, i);
		}
	      weather_info[i].wind_speed += 1;
	      break;
	    }
	}
      /*   Angle of Sunlight   */
      if (sun_light)
	{
	  roll = ((sunrise[time_info.month] + sunset[time_info.month]) / 2);

	  if (time_info.hour > roll)
	    roll =
	      (sunset[time_info.month] -
	       time_info.hour) * 100 / (sunset[time_info.month] -
					roll) * 15 / 100;
	  else if (time_info.hour == roll)
	    roll = 15;
	  else if (time_info.hour < roll)
	    roll =
	      (time_info.hour - sunrise[time_info.month]) * 100 / (roll -
								   sunrise
								   [time_info.
								    month]) *
	      15 / 100;

	  weather_info[i].temperature += roll;
	}

      /*   Cloud Chill, which applies only in the daytime - This is not scientific.   */
      if (sun_light)
	{
	  if (weather_info[i].clouds == LIGHT_CLOUDS)
	    weather_info[i].temperature -= ((roll * 3) / 10);
	  if (weather_info[i].clouds == HEAVY_CLOUDS)
	    weather_info[i].temperature -= ((roll * 6) / 10);
	  if (weather_info[i].clouds == OVERCAST)
	    weather_info[i].temperature -= ((roll * 9) / 10);
	  weather_info[i].temperature = ((weather_info[i].temperature + last_temp * 2) / 3);	/*   Limits Drastic Immediate Changes   */
	}
      else
	{
	  if ((time_info.season == SPRING) || (time_info.season == AUTUMN))	/*   Gradual Nighttime Cooling   */
	    weather_info[i].temperature -= 10;
	  else if (time_info.season == SUMMER)
	    weather_info[i].temperature -= 15;
	  else
	    weather_info[i].temperature -= 5;
	  roll = 0;
	  if (time_info.hour != sunset[time_info.month])
	    roll = 5;
	  weather_info[i].temperature =
	    ((weather_info[i].temperature + (last_temp + roll) * 4) / 5);
	  weather_info[i].temperature -= 5;	/*   Immediate Nighttime Chill   */
	}


/*        if (weather_info[i].wind_speed == BREEZE)
        {
            weather_info[i].temperature -= 10;
        }
        else if (weather_info[i].wind_speed == WINDY)
        {
            weather_info[i].temperature -= 20;
        } */

           // Wind Chill - This is FAR from scientific, but I didnt want winds to totally take over temperatures. - Koldryn
        if (weather_info[i].wind_dir == NORTH_WIND)
        {
            weather_info[i].temperature -= weather_info[i].wind_speed * 2;
            roll = 0 - weather_info[i].wind_speed * 2;
        }

        if (weather_info[i].wind_dir == WEST_WIND)
        {
            weather_info[i].temperature += (5 - weather_info[i].wind_speed * 2);
            roll = 0 + (5 - weather_info[i].wind_speed * 2);
        }

      /*   Will it rain?   */
      if (weather_info[i].state == CHANCE_RAIN)
	{
	  chance_of_rain = (weather_info[i].clouds * 15);
	  if (time_info.season == SUMMER)
	    chance_of_rain -= 20;
	  if (time_info.season == AUTUMN)
	    chance_of_rain -= 10;
	  if (time_info.season == SPRING)
	    chance_of_rain += 10;
	  chance_of_rain = MAX (1, chance_of_rain);
	  if (weather_info[i].lightning && number (0, 39))	/*   Its very rare for lightning not to cause rain   */
	    chance_of_rain = 100;

	  if (number (0, 99) < chance_of_rain)
	    weather_info[i].state = weather_info[i].clouds + 1;
	}

      /*   If its going to rain, how hard?   */
      if (weather_info[i].state > CHANCE_RAIN)
	{
	  if (weather_info[i].state > HEAVY_RAIN)
	    weather_info[i].state -= 3;
	  roll = number (0, 99);
	  if (weather_info[i].clouds == LIGHT_CLOUDS)
	    {
	      if (roll < 40)
		weather_info[i].state = CHANCE_RAIN;
	      else if (roll < 85)
		weather_info[i].state = LIGHT_RAIN;
	      else
		weather_info[i].state = STEADY_RAIN;
	    }
	  if (weather_info[i].clouds == HEAVY_CLOUDS)
	    {
	      if (roll < 30)
		weather_info[i].state = CHANCE_RAIN;
	      else if (roll < 60)
		weather_info[i].state = LIGHT_RAIN;
	      else if (roll < 90)
		weather_info[i].state = STEADY_RAIN;
	      else
		weather_info[i].state = HEAVY_RAIN;
	    }
	  if (weather_info[i].clouds == OVERCAST)
	    {
	      if (roll < 20)
		weather_info[i].state = CHANCE_RAIN;
	      else if (roll < 50)
		weather_info[i].state = LIGHT_RAIN;
	      else if (roll < 80)
		weather_info[i].state = STEADY_RAIN;
	      else
		weather_info[i].state = HEAVY_RAIN;
	    }

	  /*   Is it rain or snow?   */
	  if ((weather_info[i].temperature < 32) 
	      && (weather_info[i].state > CHANCE_RAIN))	
	    {
	      if ((weather_info[i].state += 3) == HEAVY_SNOW
		  && (weather_info[i].temperature < 20))
		weather_info[i].state--;
	    }

	  /*   Lightning should never allow existing rain to stop   */
	  if (weather_info[i].lightning 
	      && (weather_info[i].state == CHANCE_RAIN))
	    weather_info[i].state = weather_info[i].clouds + 1;
	}

      /*   If the rain has changed, display a message.   */
      if ((weather_info[i].state != last_state)
	  && (weather_info[i].state != NO_RAIN))
	{
	  if ((weather_info[i].state == CHANCE_RAIN)
	      && (last_state > CHANCE_RAIN) && (last_state < LIGHT_SNOW))
	    send_outside_zone ("The rain fades and stops.\n\r", i);
	  if ((weather_info[i].state == CHANCE_RAIN)
	      && (last_state > HEAVY_RAIN))
	    send_outside_zone ("It stops snowing.\n\r", i);
	  if (weather_info[i].state == LIGHT_RAIN)
	    send_outside_zone
	      ("A light sprinkling of rain falls from the sky.\n\r", i);
	  if (weather_info[i].state == STEADY_RAIN)
	    send_outside_zone ("A steady rain falls from the sky.\n\r", i);
	  if (weather_info[i].state == HEAVY_RAIN)
	    send_outside_zone ("Pouring rain showers down upon the land.\n\r",
			       i);
	  if (weather_info[i].state == LIGHT_SNOW)
	    send_outside_zone ("A light snow falls lazily from the sky.\n\r",
			       i);
	  if (weather_info[i].state == STEADY_SNOW)
	    send_outside_zone ("Snow falls steadily from the sky.\n\r", i);
	  if (weather_info[i].state == HEAVY_SNOW)
	    send_outside_zone ("Blinding snows fall from the sky.\n\r", i);
	}

        
        if (weather_info[i].fog)
        {
            if (weather_info[i].wind_speed == WINDY)
                weather_info[i].fog -= 1;
            if (weather_info[i].wind_speed > WINDY)
            {
                weather_info[i].fog = NO_FOG;
                send_outside_zone
                ("The fog churns in the wind and is swept away.\n\r", i);
            }
            if (time_info.hour > sunrise[time_info.month]
                    && weather_info[i].clouds == CLEAR_SKY)
                weather_info[i].fog -= 1;
            if (time_info.hour == 9)
                weather_info[i].fog -= 1;
            if (time_info.hour == 12)
                weather_info[i].fog -= 1;
            if (time_info.hour > sunrise[time_info.month]
                    && weather_info[i].clouds > CLEAR_SKY)
            {
                roll = number (1, 4);
                if (roll > weather_info[i].clouds)
                    weather_info[i].fog -= 1;
            }
            if (weather_info[i].fog < NO_FOG)
                weather_info[i].fog = NO_FOG;
            if (weather_info[i].fog == THIN_FOG && last_fog == THICK_FOG)
                send_outside_zone ("The fog thins a little.\n\r", i);
            if (weather_info[i].fog == NO_FOG)
                send_outside_zone ("The fog lifts.\n\r", i);
        }
        

        //  If its after midnight, before dawn, within 3 hours of dawn, there is no fog, and there is no artificial sunlight....
        if ((sunrise[time_info.month] < (time_info.hour + 4))
                && (sun_light == 0) && (time_info.hour < sunrise[time_info.month])
                && (weather_info[i].fog == NO_FOG))
        {
            switch (time_info.season)
            {
            case WINTER:
                chance_of_rain = 25;
                break;		//   Chance of Fog
            case AUTUMN:
                chance_of_rain = 15;
                break;
            case SPRING:
                chance_of_rain = 8;
                break;
            case SUMMER:
                chance_of_rain = 5;
                break;
            }
            roll = number (1, 100);
            if (weather_info[i].wind_speed == BREEZE && roll < chance_of_rain)
            {
                weather_info[i].fog = THIN_FOG;
                send_outside_zone ("A thin fog wafts in on the breeze.\n\r", i);
            }
            if (weather_info[i].wind_speed == CALM && roll < chance_of_rain)
            {
                weather_info[i].fog = THICK_FOG;
                send_outside_zone
                ("A thick fog begins to condense in the still air.\n\r", i);
            }
        }
        


        //if (i == 10)
        //{
        //    sprintf (buf,"%d:00 %d degrees F\n\r",time_info.hour, weather_info[i].temperature);
        //    send_to_gods (buf);
        //}


        desc_weather[i] = WR_NORMAL;

        
        if (weather_info[i].clouds > CLEAR_SKY)
            desc_weather[i] = WR_CLOUDY;

        if (weather_info[i].fog)
            desc_weather[i] = WR_FOGGY;

        if (weather_info[i].state > CHANCE_RAIN)
        {
            desc_weather[i] = WR_RAINY;
            if (weather_info[i].wind_speed == STORMY)
                desc_weather[i] = WR_STORMY;
        }

        if (weather_info[i].state > HEAVY_RAIN)
        {
            desc_weather[i] = WR_SNOWY;
            if (weather_info[i].wind_speed == STORMY)
                desc_weather[i] = WR_BLIZARD;
        }

        if (time_info.hour < sunrise[time_info.month]
                && time_info.hour > sunset[time_info.month])
        {
            switch (desc_weather[i])
            {
            case WR_NORMAL:
                desc_weather[i] = WR_NIGHT;
                break;
            case WR_FOGGY:
                desc_weather[i] = WR_NIGHT_FOGGY;
                break;
            case WR_CLOUDY:
                desc_weather[i] = WR_NIGHT;
                break;
            case WR_RAINY:
                desc_weather[i] = WR_NIGHT_RAINY;
                break;
            case WR_SNOWY:
                desc_weather[i] = WR_NIGHT_SNOWY;
                break;
            case WR_BLIZARD:
                desc_weather[i] = WR_NIGHT_BLIZARD;
                break;
            case WR_STORMY:
                desc_weather[i] = WR_NIGHT_STORMY;
                break;
            }
        }

        if (weather_info[i].clouds < HEAVY_CLOUDS
                && weather_info[i].fog < THICK_FOG
                && weather_info[i].state < HEAVY_SNOW)
        {
            if (moon_setting)
                send_outside_zone ("Ithil hangs low in the sky.\n\r", i);
            if (moon_set)
                send_outside_zone
                ("Ithil slowly sinks from the sky.\n\r",
                 i);
            if (moon_rise)
                send_outside_zone
                ("Ithil rises with stately grace into the sky.\n\r", i);
            moon_light[i] = global_moon_light;
        }
        else
        {
            moon_light[i] = 0;
        }
        
    }
}

int
is_leap_year (int year)
{
    if (year % 4 == 0)
    {
        if (year % 100 == 0)
        {
            if (year % 400 == 0)
                return 1;
            else
                return 0;
        }
        return 1;
    }

    return 0;
}
/*
void
weather_and_time (int mode)
{
    int moon_rising = 0;
    int moon_setting = 0;
    int moon_set = 0;
    int moon_q;
    int d_day;
    int i;
    int zone;
    bool new_day = false;
	
	 // Nimrod commented out from here down for TIME (approximately 97 lines)
	 /*
	 next_hour_update += 900;	// This is a mud hour; 60*60/4 

    sun_light = 1;
    //global_moon_light = 0; 
	
	
     time_info.hour++;

    int true_hour = time_info.hour + (84 * time_info.day);

    // We have eight "phases" of 84 hours on the moon to make up our month.

	if (true_hour < 84)
		time_info.phaseEarth = PHASE_FULL_EARTH;
	else if (true_hour < 168)
		time_info.phaseEarth = PHASE_GIBBOUS_WANING;
	else if (true_hour < 252)
		time_info.phaseEarth = PHASE_THREE_QUARTER;
	else if (true_hour < 336)
		time_info.phaseEarth = PHASE_CRESCENT_WANING;
	else if (true_hour < 420)
		time_info.phaseEarth = PHASE_NEW_EARTH;
	else if (true_hour < 504)
		time_info.phaseEarth = PHASE_CRESCENT_WAXING;
	else if (true_hour < 588)
		time_info.phaseEarth = PHASE_FIRST_QUARTER;
	else if (true_hour < 672)
		time_info.phaseEarth = PHASE_GIBBOUS_WAXING;
	else
		time_info.phaseEarth = PHASE_TERRA_ECLIPSE;


	if (time_info.hour >= 84)
    {
        time_info.day++;
        time_info.hour = 0;
        new_day = true;
    }

    // Once we get to gibbous waxing, it's time to come back to Full Earth.
    if (time_info.phaseEarth > PHASE_GIBBOUS_WAXING)
        time_info.phaseEarth = PHASE_FULL_EARTH;

    if (time_info.day >= 8 && new_day)
    {
        if (!time_info.holiday)
            time_info.month++;

        if (time_info.month >= 13)
        {
            time_info.month = 0;
            time_info.year++;
        }
        time_info.day = 0;
    }

    if (time_info.month >= 0 && time_info.month<= 5)
        time_info.season = SEASON_NEAR;
    else
        time_info.season = SEASON_FAR;


    // We'll calculate where the sun should be by first seeing how many hours
    // have passed since the start of our lunar "month"
    int sunCount = time_info.day * 84 + time_info.hour;
    int sunPhase = PHASE_SET;

    if (sunCount >= 612 - 1)
        sunPhase = PHASE_SET;
    // At 24.5 earth day, we start to set (24.5 * 24). We go off by one to avoid clashing with our earth phase.
    else if (sunCount >= 588 - 1)
        sunPhase = PHASE_EVENING;
    // 23.5 is dusk
    else if (sunCount >= 564 - 1)
        sunPhase = PHASE_DUSK;
    // 14 is high sun
    else if (sunCount >= 336 - 1)
        sunPhase = PHASE_MIDDAY;
    // 9 is risen
    else if (sunCount >= 216 - 1)
        sunPhase = PHASE_RISEN;
    // 8 is dawn
    else if (sunCount >= 192 - 1)
        sunPhase = PHASE_DAWN;
    // 7 is predawn
    else if (sunCount >= 168 - 1)
        sunPhase = PHASE_PREDAWN;
    else
        sunPhase = PHASE_SET;

    time_info.phaseSun = sunPhase;

    //if (sunrise[time_info.month] <= time_info.hour && sunset[time_info.month] > time_info.hour)
    sun_light = 1;
	


    for (zone = 0; zone <= 99; zone++)
    {
        switch (sunCount)
        {
        case 167:
            send_outside_zone("The faint glimmerings of sunlight can be seen over the eastern horizon\n", zone);
            break;
        case 191:
            send_outside_zone("The Sun finally begins to pull itself up over the eastern horizon.\n", zone);
            break;
        case 215:
            send_outside_zone("The Sun has now broken free of the horizon, and is reaching slowly towards the centre of the sky.\n", zone);
            break;
        case 335:
            send_outside_zone("The Sun now hovers directly overhead in the centre of the sky.\n", zone);
            break;
        case 563:
            send_outside_zone("The Sun now hangs close to the western horizon.\n", zone);
            break;
        case 587:
            send_outside_zone("The Sun begins to dip below the western horizon.\n", zone);
            break;
        case 611:
            send_outside_zone("The last of the Sun retreats beneath the western horizon, only the dim glow of its rays remaining.\n", zone);
            break;
        }

        if (new_day)
        {
            switch (time_info.phaseEarth)
            {
            case PHASE_FULL_EARTH:
                send_outside_zone("The full majesty of the Earth is now visible.\n", zone);
                break;
            case PHASE_GIBBOUS_WANING:
                send_outside_zone("The Earth begins to turn away from the moon.\n", zone);
                break;
            case PHASE_THREE_QUARTER:
                send_outside_zone("A quarter of the Earth is plunged in to darkness.\n", zone);
                break;
            case PHASE_CRESCENT_WANING:
                send_outside_zone("A thin sliver is all that remains of the Earth.\n", zone);
                break;
            case PHASE_NEW_EARTH:
                send_outside_zone("The Earth is invisible now against the blaring rays of the Sun.\n", zone);
                break;
            case PHASE_CRESCENT_WAXING:
                send_outside_zone("The edges of the Earth begin to appear.\n", zone);
                break;
            case PHASE_FIRST_QUARTER:
                send_outside_zone("A full quarter of the Earth is now visible.\n", zone);
                break;
            case PHASE_GIBBOUS_WAXING:
                send_outside_zone("More and more of the Earth slowly comes in to view.\n", zone);
                break;

            }

        }
        /*
        if (sunrise[time_info.month] == time_info.hour + 1)
        {
            if (strcmp(zone_table[zone].dawn, "default") && strcmp(zone_table[zone].dawn, "(null)"))
                send_outside_zone(zone_table[zone].dawn, zone);
            else
                send_outside_zone ("A glow illuminates the eastern horizon.\n\r", zone);
        }
        
    }

    if (is_room_affected (world_affects, MAGIC_WORLD_SOLAR_FLARE))
        sun_light = 1;

    if (is_room_affected (world_affects, MAGIC_WORLD_CLOUDS))
        sun_light = 0;

    if (is_room_affected (world_affects, MAGIC_WORLD_MOON))
        global_moon_light = 1;

Nimrod commented out down to here for time 

//    weather (moon_setting, moon_rising, moon_set);
// }
*/
void
weather_and_time (int mode)
{
	int moon_rising = 0;
	int moon_setting = 0;
	int moon_set = 0;
	int moon_q;
	int d_day;
	int i;
	bool new_day = false;

	next_hour_update += 900;	/* This is a mud hour; 60*60/4 */

	sun_light = 0;
	/*global_moon_light = 0; */

	time_info.hour++;

	if (time_info.hour >= 24)
	{
		time_info.day++;
		time_info.hour = 0;
		time_info.dayofweek++;
		new_day = true;
	}
	
	if (time_info.dayofweek >= 7)
	  time_info.dayofweek = 0;

	if (time_info.day >= 30 && new_day)
	{
		if (!time_info.holiday)
			time_info.month++;
		if (time_info.month >= 12)
		{
			time_info.month = 0;
		}
		if (time_info.month == 0 || time_info.month == 1
			|| time_info.month == 11)
			time_info.season = WINTER;
		else if (time_info.month >= 2 && time_info.month <= 4)
			time_info.season = SPRING;
		else if (time_info.month > 4 && time_info.month <= 7)
			time_info.season = SUMMER;
		else if (time_info.month > 7 && time_info.month <= 10)
			time_info.season = AUTUMN;
		
		if (time_info.holiday == HOLIDAY_METTARE)
		{
			time_info.holiday = HOLIDAY_YESTARE;
			time_info.year++;
		}
		else if (time_info.holiday == HOLIDAY_LOENDE
				 && is_leap_year (time_info.year))
			time_info.holiday = HOLIDAY_ENDERI;
		else if (time_info.holiday == HOLIDAY_ENDERI)
		{
			time_info.holiday = 0;
			time_info.day = 0;
		}
		else if (time_info.month == 0)
		{
			if (time_info.holiday != HOLIDAY_YESTARE)
				time_info.holiday = HOLIDAY_METTARE;
			else
			{
				time_info.holiday = 0;
				time_info.day = 0;
			}
		}
		else if (time_info.month == 3)
		{
			if (time_info.holiday != HOLIDAY_TUILERE)
				time_info.holiday = HOLIDAY_TUILERE;
			else
			{
				time_info.holiday = 0;
				time_info.day = 0;
			}
		}
		else if (time_info.month == 6)
		{
			if (time_info.holiday != HOLIDAY_LOENDE)
				time_info.holiday = HOLIDAY_LOENDE;
			else
			{
				time_info.holiday = 0;
				time_info.day = 0;
			}
		}
		else if (time_info.month == 9)
		{
			if (time_info.holiday != HOLIDAY_YAVIERE)
				time_info.holiday = HOLIDAY_YAVIERE;
			else
			{
				time_info.holiday = 0;
				time_info.day = 0;
			}
		}
		else
			time_info.day = 0;
	}

	if (time_info.month >= 12)
	{
		time_info.year++;
		time_info.month = 0;
	}

	if (sunrise[time_info.month] <= time_info.hour &&
		sunset[time_info.month] > time_info.hour)
		sun_light = 1;

	if (sunrise[time_info.month] == time_info.hour + 1)
		send_outside ("A glow illuminates the eastern horizon.\n\r");

	if (sunrise[time_info.month] == time_info.hour)
		send_outside
		("Anor's fiery exterior slowly lifts itself up over the eastern horizon beneath Arien's unwavering guidance.\n\r");

	if (sunset[time_info.month] == time_info.hour + 1)
		send_outside
		("Anor begins dipping below the western horizon, guided to its respite by Arien.\n\r");

	if (sunset[time_info.month] == time_info.hour)
		send_outside
		("Anor sets in a fiery cascade of brilliant color upon the western horizon.\n\r");

	d_day = (time_info.day + 15) % 30;
	moon_q = d_day * 24 / 30;

	for (i = -7; i <= 6; i++)
	{
		if (moon_q == (24 + time_info.hour + i) % 24)
		{
			if (i == -7)
			{
				moon_set = 1;
				global_moon_light = 0;
			}
			/*else
			global_moon_light = 1; */

			if (i == -6)
			{
				moon_setting = 1;
			}
			if (i == 6)
			{
				moon_rising = 1;
				global_moon_light = (time_info.hour > 16
					&& time_info.hour < 20) ? 1 : 0;
			}
		}
	}

	if (is_room_affected (world_affects, MAGIC_WORLD_SOLAR_FLARE))
		sun_light = 1;

	if (is_room_affected (world_affects, MAGIC_WORLD_CLOUDS))
		sun_light = 0;

	if (is_room_affected (world_affects, MAGIC_WORLD_MOON))
		global_moon_light = 1;

	weather (moon_setting, moon_rising, moon_set);
}


int
weather_object_exists(OBJ_DATA * list, int vnum)
{
    for (; list; list = list->next_content)
        if (list->nVirtual == vnum)
            return 1;

    return 0;
}

const int weather_objects[12] =
{
    500, 
    501,
    502,
    503,
    504,
    505,
    506,
    507,
    508,
    509,
    510,
    511
};


/*
load_weather_obj()

This function is called every time do_look is called for a room
and checks the weather in room being looked at to see if the appropriate
objects for the current weather are present and if not loads them as required.

The objects

*/
void
load_weather_obj(ROOM_DATA *troom)
{
    OBJ_DATA *obj = NULL;
    //int number = 0;

    // Return on indoors, fall, climb, or water rooms.

    if (IS_SET (troom->room_flags, INDOORS)
            || IS_SET (troom->room_flags, FALL)
            || IS_SET (troom->room_flags, CLIMB)
            || troom->sector_type == SECT_RIVER
            || troom->sector_type == SECT_LAKE
            || troom->sector_type == SECT_OCEAN
            || troom->sector_type == SECT_REEF
            || troom->sector_type == SECT_UNDERWATER)
        return;

    /* If it is raining */
    if ((weather_info[troom->zone].state > CHANCE_RAIN)
            &&(weather_info[troom->zone].state < LIGHT_SNOW))
    {
        //if there is snow in the room replace it with slush
        if ((weather_object_exists(troom->contents, weather_objects[0])) ||
                (weather_object_exists(troom->contents, weather_objects[1])) ||
                (weather_object_exists(troom->contents, weather_objects[10])) ||
                (weather_object_exists(troom->contents, weather_objects[11])))
        {
            //remove snow from room
            if ((obj = get_obj_in_list_num (weather_objects[0], troom->contents)))
                extract_obj(obj);
            if ((obj = get_obj_in_list_num (weather_objects[1], troom->contents)))
                extract_obj(obj);
            if ((obj = get_obj_in_list_num (weather_objects[10], troom->contents)))
                extract_obj(obj);
            if ((obj = get_obj_in_list_num (weather_objects[11], troom->contents)))
                extract_obj(obj);

            // also remove any puddles we do have here, as they can be there under
            // some circumstances.
            if ((obj = get_obj_in_list_num (weather_objects[3], troom->contents)))
                extract_obj(obj);
            if ((obj = get_obj_in_list_num (weather_objects[4], troom->contents)))
                extract_obj(obj);

            //load slush in room
            obj = load_object(weather_objects[2]);
            obj_to_room (obj, troom->vnum);
        }
        else
        {
            //if there is ash, replace it with muddy ash
            if ((weather_object_exists(troom->contents, weather_objects[5]))||
                    (weather_object_exists(troom->contents, weather_objects[6])))
            {
                //remove ash
                if ((obj = get_obj_in_list_num (weather_objects[5], troom->contents)))
                    extract_obj(obj);
                if ((obj = get_obj_in_list_num (weather_objects[6], troom->contents)))
                    extract_obj(obj);

                //if not already muddy ash in the room load some
                if (!(obj = get_obj_in_list_num (weather_objects[0], troom->contents)))
                {
                    obj = load_object(weather_objects[9]);
                    obj_to_room (obj, troom->vnum);
                }
            }
            else
            {
                //if there are old puddles replace them with new ones
                if (weather_object_exists(troom->contents, weather_objects[4]))
                {
                    //remove old puddles from room
                    if ((obj = get_obj_in_list_num (weather_objects[4], troom->contents)))
                        extract_obj(obj);

                    //load fresh puddles in room
                    obj = load_object(weather_objects[3]);
                    obj_to_room (obj, troom->vnum);
                }
                else
                {
                    //if there are no puddles or slush or muddy ash load puddles
                    if ((!weather_object_exists(troom->contents, weather_objects[3]))&&
                            (!weather_object_exists(troom->contents, weather_objects[2]))&&
                            (!weather_object_exists(troom->contents, weather_objects[9])))
                    {
                        //load fresh puddles in room
                        obj = load_object(weather_objects[3]);
                        obj_to_room (obj, troom->vnum);
                    }
                }
            }
        }
    }
    else if (weather_info[troom->zone].state > HEAVY_RAIN)
    {
        //if there is rain in the room replace it with slush
        if ((weather_object_exists(troom->contents, weather_objects[3]))||
                (weather_object_exists(troom->contents, weather_objects[4])))
        {
            //remove rain from room
            if ((obj = get_obj_in_list_num (weather_objects[3], troom->contents)))
                extract_obj(obj);
            if ((obj = get_obj_in_list_num (weather_objects[4], troom->contents)))
                extract_obj(obj);

            //load slush in room
            obj = load_object(weather_objects[2]);
            obj_to_room (obj, troom->vnum);
        }
        else
        {
            //if there is ash, replace it with muddy ash
            if ((weather_object_exists(troom->contents, weather_objects[5]))||
                    (weather_object_exists(troom->contents, weather_objects[6])))
            {
                //remove ash
                if ((obj = get_obj_in_list_num (weather_objects[5], troom->contents)))
                    extract_obj(obj);
                if ((obj = get_obj_in_list_num (weather_objects[6], troom->contents)))
                    extract_obj(obj);

                //if not already muddy ash in the room load some
                if (!(obj = get_obj_in_list_num (weather_objects[9], troom->contents)))
                {
                    obj = load_object(weather_objects[9]);
                    obj_to_room (obj, troom->vnum);
                }
            }
            else
            {
                if (weather_info[troom->zone].state == LIGHT_SNOW)
                {
                    //if there is old snow replace it with new snow
                    if (weather_object_exists(troom->contents, weather_objects[1]))
                    {
                        //remove old snow from room
                        if ((obj = get_obj_in_list_num (weather_objects[1], troom->contents)))
                            extract_obj(obj);

                        //load fresh snow in room
                        obj = load_object(weather_objects[0]);
                        obj_to_room (obj, troom->vnum);
                    }

                    // If there's a heavy blanket of snow, then top it up.

                    else if (weather_object_exists(troom->contents, weather_objects[10]) ||
                             weather_object_exists(troom->contents, weather_objects[11]))
                    {
                        if ((obj = get_obj_in_list_num (weather_objects[1], troom->contents)))
                            extract_obj(obj);
                        if ((obj = get_obj_in_list_num (weather_objects[10], troom->contents)))
                            extract_obj(obj);
                        if ((obj = get_obj_in_list_num (weather_objects[11], troom->contents)))
                            extract_obj(obj);

                        obj = load_object(weather_objects[10]);
                        obj_to_room (obj, troom->vnum);

                    }
                    else
                    {
                        //if there is no snow or slush or muddy ash load fresh snow
                        if ((!weather_object_exists(troom->contents, weather_objects[2]))&&
                                (!weather_object_exists(troom->contents, weather_objects[0]))&&
                                (!weather_object_exists(troom->contents, weather_objects[9])))
                        {
                            //load fresh snow in room
                            obj = load_object(weather_objects[0]);
                            obj_to_room (obj, troom->vnum);
                        }
                    }
                }
                else if (weather_info[troom->zone].state > LIGHT_SNOW)
                {
                    //if there is snow or rain or slush, replace it with muddy ash
                    if ((weather_object_exists(troom->contents, weather_objects[0]))||
                            (weather_object_exists(troom->contents, weather_objects[1]))||
                            (weather_object_exists(troom->contents, weather_objects[2]))||
                            (weather_object_exists(troom->contents, weather_objects[3]))||
                            (weather_object_exists(troom->contents, weather_objects[4]))||
                            (weather_object_exists(troom->contents, weather_objects[5]))||
                            (weather_object_exists(troom->contents, weather_objects[6]))||
                            (weather_object_exists(troom->contents, weather_objects[9]))||
                            (weather_object_exists(troom->contents, weather_objects[11])))
                    {
                        //remove snow/rain/slush
                        if ((obj = get_obj_in_list_num (weather_objects[0], troom->contents)))
                            extract_obj(obj);
                        if ((obj = get_obj_in_list_num (weather_objects[1], troom->contents)))
                            extract_obj(obj);
                        if ((obj = get_obj_in_list_num (weather_objects[2], troom->contents)))
                            extract_obj(obj);
                        if ((obj = get_obj_in_list_num (weather_objects[3], troom->contents)))
                            extract_obj(obj);
                        if ((obj = get_obj_in_list_num (weather_objects[4], troom->contents)))
                            extract_obj(obj);
                        if ((obj = get_obj_in_list_num (weather_objects[5], troom->contents)))
                            extract_obj(obj);
                        if ((obj = get_obj_in_list_num (weather_objects[6], troom->contents)))
                            extract_obj(obj);
                        if ((obj = get_obj_in_list_num (weather_objects[9], troom->contents)))
                            extract_obj(obj);
                        if ((obj = get_obj_in_list_num (weather_objects[11], troom->contents)))
                            extract_obj(obj);
                    }

                    if (!weather_object_exists(troom->contents, weather_objects[10]))
                    {
                        obj = load_object(weather_objects[10]);
                        obj_to_room (obj, troom->vnum);
                    }

                }
            }
        }
    }
    
    /* If volcanic smoke is set */

    if (weather_info[troom->zone].special_effect == VOLCANIC_SMOKE)
    {
        //if there is snow or rain or slush, replace it with muddy ash
        if ((weather_object_exists(troom->contents, weather_objects[0]))||
                (weather_object_exists(troom->contents, weather_objects[1]))||
                (weather_object_exists(troom->contents, weather_objects[2]))||
                (weather_object_exists(troom->contents, weather_objects[3]))||
                (weather_object_exists(troom->contents, weather_objects[4])))
        {
            //remove snow/rain/slush
            if ((obj = get_obj_in_list_num (weather_objects[0], troom->contents)))
                extract_obj(obj);
            if ((obj = get_obj_in_list_num (weather_objects[1], troom->contents)))
                extract_obj(obj);
            if ((obj = get_obj_in_list_num (weather_objects[2], troom->contents)))
                extract_obj(obj);
            if ((obj = get_obj_in_list_num (weather_objects[3], troom->contents)))
                extract_obj(obj);
            if ((obj = get_obj_in_list_num (weather_objects[4], troom->contents)))
                extract_obj(obj);

            //if not already muddy ash in the room load some
            if (!(obj = get_obj_in_list_num (weather_objects[9], troom->contents)))
            {
                obj = load_object(weather_objects[9]);
                obj_to_room (obj, troom->vnum);
            }
        }
        else
        {
            //if there is old ash replace it with new ash
            if (weather_object_exists(troom->contents, weather_objects[6]))
            {
                //remove old ash from room
                if ((obj = get_obj_in_list_num (weather_objects[6], troom->contents)))
                    extract_obj(obj);

                //load fresh ash in room
                obj = load_object(weather_objects[5]);
                obj_to_room (obj, troom->vnum);
            }
            else
            {
                //if there is no ash or muddy ash load it
                if ((!weather_object_exists(troom->contents, weather_objects[5]))&&
                        (!weather_object_exists(troom->contents, weather_objects[9])))
                {
                    //load fresh ash in room
                    obj = load_object(weather_objects[5]);
                    obj_to_room (obj, troom->vnum);
                }
            }
        }
    }

    /* If foul stench is set */
    if (weather_info[troom->zone].special_effect == FOUL_STENCH)
    {
        //if there is old stench replace it with new stench
        if (weather_object_exists(troom->contents, weather_objects[8]))
        {
            //remove old stench from room
            if ((obj = get_obj_in_list_num (weather_objects[8], troom->contents)))
                extract_obj(obj);

            //load fresh stench in room
            obj = load_object(weather_objects[7]);
            obj_to_room (obj, troom->vnum);
        }
        else
        {
            //if there is no stench load it
            if ((!weather_object_exists(troom->contents, weather_objects[7])))
            {
                //load fresh stench in room
                obj = load_object(weather_objects[7]);
                obj_to_room (obj, troom->vnum);
            }
        }
    }
}


char *
short_time_string (int day, int month)
{
    char buf[MAX_STRING_LENGTH] = { '\0' };
    char buf2[MAX_STRING_LENGTH] = { '\0' };
    char buf3[MAX_STRING_LENGTH] = { '\0' };
	char buf4[MAX_STRING_LENGTH] = { '\0' };
    static char short_time_str[MAX_STRING_LENGTH] = { '\0' };
	
	

	// sprintf (buf, "%s-%d", abrev_earth_phase[day], month+1);

    sprintf (buf, "%s %d", short_month_name[month], day); // change to weekday instead of abrev_earth_phase and month_name[] instead of a number.

    sprintf (short_time_str, "%s", buf);

    return short_time_str;
}


/*

sniff... no one appreciated LToB's quenyan calendar

int
elf_season (int *xday, int month)
{
  int xmonth = 0;

  xmonth = month + 1;

  switch(xmonth)
  {
    case 3:
      xmonth = 0;
      break;
    case 4:
      if (*xday <= 24)
      {
        xmonth = 0;
        xday += 30;
      }
      else
      {
        xday -= 24;
        xmonth = 1;
      }
      break;
    case 5:
      xday += 6;
      xmonth = 1;
      break;
    case 6:
      xday += 36;
      xmonth = 1;
      break;
    case 7:
      if (*xday <= 6)
      {
        xmonth = 1;
        xday += 66;
      }
      else
      {
        xday -= 6;
        xmonth = 2;
      }
      break;
    case 8:
      xday += 24;
      xmonth = 2;
      break;
    case 9:
        xmonth = 3;
      break;
    case 10:
      if (*xday <= 24)
      {
        xmonth = 3;
        xday += 30;
      }
      else
      {
        xday -= 24;
        xmonth = 4;
      }
      break;
    case 11:
      xday += 6;
      xmonth = 4;
      break;
    case 12:
      xday += 36;
      xmonth = 4;
      break;
    case 1:
      if (*xday <= 6)
      {
        xmonth = 4;
        xday += 66;
      }
      else
      {
        xday -= 6;
        xmonth = 5;
      }
      break;
    case 2:
      xmonth = 5;
      xday += 24;
  }

  return xmonth;
}
*/
