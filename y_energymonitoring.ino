/***
   emon_data

   0 - real power
   1 - apparent power
   2 - power factor
   3 - supply voltage/Vrms
   4 - irms
*/
void init_emon()
{
  emon1.voltage(19, 234.26, 1.7);  // Voltage: input pin, calibration, phase_shift
  emon1.current(18, 111.1);       // Current: input pin, calibration.
}

void read_volt_cur()
{
  emon1.calcVI(20,2000);         // Calculate all. No.of half wavelengths (crossings), time-out
  emon1.serialprint();           // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)
  
  emon_data[0]   = emon1.realPower;        //extract Real Power into variable
  emon_data[1]   = emon1.apparentPower;    //extract Apparent Power into variable
  emon_data[2]   = emon1.powerFactor;      //extract Power Factor into Variable
  emon_data[3]   = emon1.Vrms;             //extract Vrms into Variable
  emon_data[4]   = emon1.Irms;             //extract Irms into Variable
}
void display_data(float power, float e_usage, float e_cost)
{
  Serial.print("Power : ");
  Serial.print(power);
  Serial.println(" W");
  Serial.print("Energy Usage : ");
  Serial.print(e_usage);
  Serial.println(" kWh");
  Serial.print("Energy Cost : ");
  Serial.print(e_cost);
  Serial.println(" pesos");
}

float calculate_power() // watts
{
  return (((abs(emon_data[3])*abs(emon_data[4]))*(emon_data[2]))/1000);
}

float energy_usage(float usage_hours) // kWh
{
  return ((((abs(emon_data[3]))*(abs(emon_data[4])))*(emon_data[2]))*(usage_hours));
}

float energy_cost(float energy_usage,float cost_per_kwh)
{
  return energy_usage*cost_per_kwh;
}
