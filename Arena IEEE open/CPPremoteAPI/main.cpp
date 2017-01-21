/*
    Client of V-REP simulation server (remoteApi)
    Copyright (C) 2015  Rafael Alceste Berri rafaelberri@gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Habilite o server antes na simulação V-REP com o comando lua:
// simExtRemoteApiStart(portNumber) -- inicia servidor remoteAPI do V-REP

extern "C" {
  #include "remoteApi/extApi.h"
}

#include <iostream>
#include <string>

using namespace std;

int main(int argc, char **argv)
{
  string serverIP = "127.0.0.1";
  int serverPort = 19999;
  float vLeft = 0;
  float vRight = 0;
  string sensorNome[6];
  int proximity_sensor[6];
  // MOTORES
  int motor_tras_direito = 0;
  int motor_tras_esquerdo = 0;
  int motor_frente_direito =0;
  int motor_frente_esquerdo =0;


  // variaveis de cena e movimentação do mamador
  float noDetectionDist=0.5;
  float maxDetectionDist=0.2;
  float detect[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  float braitenbergL[16]={-0.2,-0.4,-0.6,-0.8,-1,-1.2,-1.4,-1.6, 0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  float braitenbergR[16]={-1.6,-1.4,-1.2,-1,-0.8,-0.6,-0.4,-0.2, 0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  float v0=2;

  int clientID=simxStart((simxChar*)serverIP.c_str(),serverPort,true,true,2000,5);

  if (clientID!=-1)
  {
    cout << "Servidor conectado!" << std::endl;

    // inicialização dos motores traseiros                       NOME DO JOIN MOTOR\/    AONDE ELE VAI GUARDAR \/
    if(simxGetObjectHandle(clientID,(const simxChar*) "Motor_esquerdo_Tras",(simxInt *) &motor_tras_esquerdo, (simxInt) simx_opmode_oneshot_wait) != simx_return_ok)
      cout << "Motor_esquerdo_Tras nao encontrado!" << std::endl;
    else
      cout << "Conectado ao Motor_esquerdo_Tras!" << std::endl;

    if(simxGetObjectHandle(clientID,(const simxChar*) "Motor_direito_Tras",(simxInt *) &motor_tras_direito, (simxInt) simx_opmode_oneshot_wait) != simx_return_ok)
      cout << "Motor_direito_Tras nao encontrado!" << std::endl;
    else
      cout << "Conectado ao Motor_direito_Tras!" << std::endl;

      // inicialização dos motores da frente     OBS:NÃO UTILIZEI NO CODIGO
    if(simxGetObjectHandle(clientID,(const simxChar*) "Motor_esquerdo_frente",(simxInt *) &motor_frente_esquerdo, (simxInt) simx_opmode_oneshot_wait) != simx_return_ok)
      cout << "Motor_esquerdo_Frente nao encontrado!" << std::endl;
    else
      cout << "Conectado ao Motor_esquerdo_Frente!" << std::endl;

    if(simxGetObjectHandle(clientID,(const simxChar*) "Motor_direito_frente",(simxInt *) &motor_frente_direito, (simxInt) simx_opmode_oneshot_wait) != simx_return_ok)
      cout << "Motor_direito_Frente nao encontrado!" << std::endl;
    else
      cout << "Conectado ao Motor_direito_Frente!" << std::endl;

    // inicialização dos sensores (remoteApi)
    for(int i = 0; i < 6; i++)
    {
      sensorNome[i] = "Proximity_sensor_" + to_string(i + 1);

      if(simxGetObjectHandle(clientID,(const simxChar*) sensorNome[i].c_str(),(simxInt *) &proximity_sensor[i], (simxInt) simx_opmode_oneshot_wait) != simx_return_ok)
	cout << "Proximity_sensor_" << sensorNome[i] << " nao encontrado!" << std::endl;
      else
      {
        cout << "Conectado ao sensor " << sensorNome[i] << std::endl;
	simxReadProximitySensor(clientID,proximity_sensor[i],NULL,NULL,NULL,NULL,simx_opmode_streaming);
      }
    }


    while(simxGetConnectionId(clientID)!=-1) // enquanto a simulação estiver ativa
    {
      for(int i = 0; i < 16; i++)
      {
	simxUChar state;
	simxFloat coord[3]; //cordenada x,y,z

        // LENDO OS SENSORES             \/ Variavel que você declarou
	if (simxReadProximitySensor(clientID,proximity_sensor[i],&state,coord,NULL,NULL,simx_opmode_buffer)==simx_return_ok)
	{
	  float dist = coord[2]; //    no caso, pegamos as informações da cordenada   Z
	  if(state > 0 && (dist<noDetectionDist)) //noDetectionDist = 0.5
	  {
	    if(dist<maxDetectionDist)
	    {
	      dist=maxDetectionDist;
	    }

	    detect[i]=1-((dist-maxDetectionDist)/(noDetectionDist-maxDetectionDist)); // calculo copiado do pioneer
	  }
	  else
	    detect[i] = 0;
	}
	else
	  detect[i] = 0;
      }

      vLeft = v0;
      vRight = v0;

      for(int i = 0; i < 16; i++)
      {
	vLeft=vLeft+braitenbergL[i]*detect[i];
        vRight=vRight+braitenbergR[i]*detect[i];
      }

      // atualiza velocidades dos motores (motores da frente, só ativar)
      simxSetJointTargetVelocity(clientID, motor_tras_esquerdo, (simxFloat) vLeft, simx_opmode_streaming);
      simxSetJointTargetVelocity(clientID, motor_tras_direito, (simxFloat) vRight, simx_opmode_streaming);
/*    simxSetJointTargetVelocity(clientID, motor_frente_esquerdo_direito, (simxFloat) vRight, simx_opmode_streaming);
      simxSetJointTargetVelocity(clientID, motor_frente_esquerdo_direito, (simxFloat) vRight, simx_opmode_streaming);
*/

      // espera um pouco antes de reiniciar a leitura dos sensores
      extApi_sleepMs(5);
    }

    simxFinish(clientID); // fechando conexao com o servidor
    cout << "Conexao fechada!" << std::endl;
  }
  else
    cout << "Problemas para conectar o servidor!" << std::endl;
  return 0;
}