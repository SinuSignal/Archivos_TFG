#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#define LEN 256
#define MAX_CMN_LEN 100
/*
DECIMAL TO HEXADECIMAL CONVERTER
*/
char  decToHexa(int quo)
{

  long int decNum,rem;
  int i=1,j,temp;
  char hexadecNum[100];

  while(quo!=0)
  {
  temp = quo % 16;
  if( temp < 10)
  temp =temp + 48; else
  temp = temp + 55;
  hexadecNum[i++]= temp;
  quo = quo / 16;
  }
  //printf("hexadecimal value of decimal number entered is %d: ",decNum);
  for (j = i -1 ;j> 0;j--)
  //printf("%c\n",hexadecNum[j]);

  return hexadecNum[1];
}

/*
CREATE DE TEMPORAL FILE OF DHCP CONFIGURATION.
NOT REQUIRED
*/

int writeFile(char * addr)
{
   FILE * fp;
   int i;
   //char * ip="192.168.1.100";
   /* open the file for writing*/
   char cwd[PATH_MAX];
   if (getcwd(cwd, sizeof(cwd)) != NULL){
     printf("%s\n",cwd );
   }else{
     return 0;
   }
   strcat(cwd,"/dhclient.conf");
   printf("%s\n",cwd );

   //strcpy(cwd,"/etc/dhcp/dhcli.conf");
   fp = fopen (cwd,"w");

   /* write 10 lines of text into the file stream*/

       fprintf (fp, "option rfc3442-classless-static-routes code 121 = array of unsigned integer 8;\n");
       fprintf(fp, "send host-name = gethostname();\n");
       fprintf(fp, "request subnet-mask, broadcast-address, time-offset, routers,\n\t domain-name, domain-name-servers, domain-search, host-name,\n\t dhcp6.name-servers, ");
       fprintf(fp, "dhcp6.domain-search, dhcp6.fqdn, dhcp6.sntp-servers,\n\t netbios-name-servers, netbios-scope, interface-mtu,\n\t rfc3442-classless-static-routes, ntp-servers;\n" );
       fprintf(fp, "send dhcp-lease-time 3600;\n");
       fprintf(fp, "send dhcp-client-identifier 01:1a:2b:3c:4d:5e:66;\n");
       fprintf(fp, "timeout 300;\n" );
       if(addr!=NULL){
       //fprintf(fp, "#" );
       fprintf(fp,"send dhcp-requested-address %s;\n",addr );
        }
   /* close the file*/
   fclose (fp);
   return 0;
}
/*
RANDOM MAC GENERATOR.
*/
char * getRandomMac(char * mac){

    int i,j =0;
    //char mac[17];


    for(i=0;i<17;i++){
        //mac[i]=decToHexa((int)rand() % 16);
        mac[i]=decToHexa((int)random() % 16);
        if(mac[i]<1)
          mac[i]='0';
    }

    mac[1]='2';
    mac[2]=':';
    mac[5]=':';
    mac[8]=':';
    mac[11]=':';
    mac[14]=':';
    //mac[17]='\0';
  return mac;
}
/*
FUNCTION TO WRITE INPUT IN COMMAND LINE.
*/
void writeCmd(char * input, char ** output){

  FILE *pf;
  pf = popen(input,"r");

  fgets(*output, 100 , pf);

  if (pclose(pf) != 0)
      fprintf(stderr," Error: Failed to close command stream \n");

    //return output;
}
/*
RETURNS THE NAME OF THE CURRENT WIFI INTERFACE
*/
void getDev(char ** dev){

char * com=(char *)calloc(250,sizeof(char));
char * tmp=(char *)calloc(100,sizeof(char));
//strcpy(tmp,*dev);
strcpy(com,"nmcli device | grep -E \"(^| )wifi( |$)\" | awk '{print $1}'");
writeCmd(com,&tmp);
tmp[strlen(tmp)-1]='\0';
strcpy(*dev,tmp);
//printf("%ld\n", strlen(*dev));
free(tmp);
free(com);

}
/*
FUCTION TO GET INFORMATION ABOUT CURRENT CONNECTION.
DATE,HOUR,ESSID,MAC,BSIID,IP
*/
void infomac(char * outfile,char * dev){
  char * com6=(char *)calloc(250,sizeof(char));
  char * data=(char *)calloc(250,sizeof(char));
  strcpy(com6,"echo \"------------\">>");
  strcat(com6,outfile);
  writeCmd(com6,&data);
  strcpy(com6,"date>>");
  strcat(com6,outfile);
  writeCmd(com6,&data);
  strcpy(com6,"iwconfig ");
  strcat(com6,dev);
  strcat(com6," | grep -e \"ESSID\\|Access Point\">>");
  strcat(com6,outfile);
  writeCmd(com6,&data);
  strcpy(com6,"ifconfig ");
  strcat(com6,dev);
  strcat(com6," | grep -w \"inet\\|ether\">>");
  strcat(com6,outfile);
  writeCmd(com6,&data);
  free(com6);
  free(data);
  return;
}


void createTmp(char * file, char * ip){

  char * com=(char *)malloc(250);
  char * tmp=(char *)malloc(100);
  strcpy(com,"sudo cp /etc/dhcp/dhclient.conf ");
  strcat(com,file);
  writeCmd(com,&tmp);

  strcpy(com,"echo 'send dhcp-requested-address ");
  strcat(com,ip);
  strcat(com,";' | sudo tee -a /etc/dhcp/dhclient_tmp.conf");
  writeCmd(com,&tmp);

  strcpy(com,"echo 'send dhcp-lease-time 3600;' | sudo tee -a /etc/dhcp/dhclient_tmp.conf");
  writeCmd(com,&tmp);

  free(com);
  free(tmp);
}
//release .100

int main(int argc, char *argv[])
{
    /*
    CONFIGURABLE PARAMETERS
    */

    char * confFile="/etc/dhcp/dhclient_tmp.conf ";//CONFIGURATION TEMPFILE UBICATION. space at end needed.
    char * wifi="lalalala";//WIFI FOR CONNECTION
    char * outInfo="infoFile.txt";//OUPUT FILE FOR INFORMATioN MAC FUNCTION
    char * ip="192.168.1.100";//IP REQUESTED IN DHCP CASES
    int sleepTime=30;//SLEEP TIME FOR LOOPS

    /**************************************************************************/

    char cmd[MAX_CMN_LEN] = "", **p;
    char mode[MAX_CMN_LEN]="";
    char option[MAX_CMN_LEN]="";
    int option_number=0;
    int i,j=0;
    FILE *pf;

    char command[100];
    char * data =(char *)malloc(100);

    char buf[1];
    char * decmac=(char *)calloc(18,sizeof(char));

    char * com6=(char *)malloc(150);
    int l=0;

    char * dev = (char *)malloc(100);
    char enter=0;

    getDev(&dev);
    struct timespec ts;
    if (timespec_get(&ts, TIME_UTC) == 0) {
      /* Handle error */
      printf("%s\n","Error asiging random seed" );
    }

    srand(time(NULL));
    srandom(ts.tv_nsec ^ ts.tv_sec);

    if (argc < 2) /*no command specified*/
    {
        fprintf(stderr, "Usage: ./privacy terminal_command ...\n");
        exit(EXIT_FAILURE);
    }
    else if(!strcmp("info",argv[1])){//segundo parametro info
        infomac(outInfo,dev);
        printf("Info about MAC address and IP in :%s\n", outInfo);

    }
    else if(!strcmp("mode",argv[1]))
    {
          //MODO


          printf("Changing address options on: %s\n",wifi );


          if(argv[2]!=NULL){
          strcat(option,argv[2]);
          printf("option: %s\n",option );

          if(!strcmp(option,"athome")){
            option_number=1;
          }else if(!strcmp(option,"dh10")){
            option_number=2;
          }else if(!strcmp(option,"diez")){
            option_number=3;
          }else if(!strcmp(option,"getCon")){
            option_number=4;
          }else if(!strcmp(option,"rmac")){
            option_number=5;
          }else if(!strcmp(option,"con")){
            option_number=6;
          }else if(!strcmp(option,"test")){
            option_number=7;
          }else if(!strcmp(option,"dhcp")){
            option_number=8;
          }else if(!strcmp(option,"release")){
            option_number=9;
          }else if(!strcmp(option,"clientID")){
            option_number=10;
          }else{
            printf("%s\n", "not balid option");
            return 0;
          }


          createTmp(confFile,ip);


          switch(option_number){

          case 1:

          printf("%s\n", "configuring low privacy");
          //No rand scan, permanente, dhcp defa, hostname, ask home wifi
          writeFile(ip);
          break;

          case 2://dh10. con ip manual pulsar para cambiar y luego release

          decmac=getRandomMac(decmac);
          strcpy(com6,"nmcli connection modify --temporary ");
          strcat(com6,wifi);
          strcat(com6," 802-11-wireless.cloned-mac-address ");
          strcat(com6,decmac);
          writeCmd(com6,&data);

          strcpy(com6,"nmcli connection modify --temporary ");
          strcat(com6,wifi);
          strcat(com6," ipv4.addresses ");
          strcat(com6,ip);
          writeCmd(com6,&data);

          strcpy(com6,"nmcli connection modify --temporary ");
          strcat(com6,wifi);
          strcat(com6," ipv4.method manual");
          writeCmd(com6,&data);

          strcpy(com6,"nmcli connection up ");
          strcat(com6,wifi);
          writeCmd(com6,&data);

          strcpy(com6,"sudo dhclient -v -cf ");
          strcat(com6,confFile);
          strcat(com6,dev);
          writeCmd(com6,&data);

          printf("Press to continue \n");
          enter=0;

          while(enter!='\r' && enter!='\n'){enter=getchar();}

          strcpy(com6,"sudo dhclient -r -v ");
          strcat(com6,dev);
          writeCmd(com6,&data);

          decmac=getRandomMac(decmac);
          strcpy(com6,"nmcli connection modify --temporary ");
          strcat(com6,wifi);
          strcat(com6," 802-11-wireless.cloned-mac-address ");
          strcat(com6,decmac);
          writeCmd(com6,&data);

          strcpy(com6,"nmcli connection up ");
          strcat(com6,wifi);
          writeCmd(com6,&data);

          strcpy(com6,"sudo dhclient -v -cf ");
          strcat(com6,confFile);
          strcat(com6,dev);
          writeCmd(com6,&data);

          printf("Press enter to release IP\n");
          enter=0;
          while(enter!='\r' && enter!='\n'){enter=getchar();}

          strcpy(com6,"sudo dhclient -r -v ");
          strcat(com6,dev);
          writeCmd(com6,&data);




          break;

          case 3://diez. Con ip rand pulsar para cambiar.

          decmac=getRandomMac(decmac);
          strcpy(com6,"nmcli connection modify --temporary ");
          strcat(com6,wifi);
          strcat(com6," 802-11-wireless.cloned-mac-address ");
          strcat(com6,decmac);
          writeCmd(com6,&data);

          strcpy(com6,"nmcli connection modify --temporary ");
          strcat(com6,wifi);
          strcat(com6," ipv4.method auto");
          writeCmd(com6,&data);
          strcpy(com6,"nmcli connection up ");
          strcat(com6,wifi);
          writeCmd(com6,&data);

          printf("Press to continue \n");
          enter=0;

          while(enter!='\r' && enter!='\n'){enter=getchar();}

          decmac=getRandomMac(decmac);
          strcpy(com6,"nmcli connection modify --temporary ");
          strcat(com6,wifi);
          strcat(com6," 802-11-wireless.cloned-mac-address ");
          strcat(com6,decmac);
          writeCmd(com6,&data);


          strcpy(com6,"nmcli connection up ");
          strcat(com6,wifi);
          writeCmd(com6,&data);

          break;

          case 4: //getCon. ver nombre del wifi conectado
          sprintf(command, "nmcli connection show --active | grep wifi | awk '{print $1}'");
          //sprintf(command, "nmcli device show | grep -B 1 -m 1 "wifi" | awk 'NR==1{print $2}'");get device
          //nmcli device show | grep -E -B 1  "(^| )wifi( |$)" | awk 'NR==1{print $2}'
          //nmcli device | grep -E "(^| )wifi( |$)" | awk '{print $1}'
          writeCmd(command, &data);

          char * new = (char *)malloc(2);
          int z=0;
          while(data[z]!='\0'){
            //printf("z%d",z );
            new[z]=data[z];
            //printf("new%s",new );
            if(strlen(new)==(z+1)){
              new=(char *)realloc(new,strlen(new)+1);
            }
            z++;
          }
          new[z-1]='\0';

          printf("You are connected to %s.\n",new );
          break;

          case 5://release MAC. Dirección que se desea hacer release por argumento.

          decmac=getRandomMac(decmac);//for free

          if(argv[3]!=NULL){
            //strcpy(rMAC,argv[3]);
            strcpy(com6,"nmcli connection modify --temporary ");
            strcat(com6,wifi);
            strcat(com6," 802-11-wireless.cloned-mac-address ");
            strcat(com6,argv[3]);
            writeCmd(com6,&data);
            strcpy(com6,"nmcli connection modify --temporary ");
            strcat(com6,wifi);
            strcat(com6," ipv4.addresses ");
            strcat(com6,ip);
            writeCmd(com6,&data);
            strcpy(com6,"nmcli connection modify --temporary ");
            strcat(com6,wifi);
            strcat(com6," ipv4.method manual");
            writeCmd(com6,&data);
            strcpy(com6,"nmcli connection up ");
            strcat(com6,wifi);
            strcpy(com6,"sudo dhclient -v -cf /etc/dhcp/dhclient_tmp.conf ");
            strcat(com6,dev);
            writeCmd(com6,&data);
            writeCmd(com6,&data);
            strcpy(com6,"sudo dhclient -r -v ");
            strcat(com6,dev);
            writeCmd(com6,&data);

          }else{
            printf("MAC to release needed\n" );
            break;
          }


          break;


          case 6://con. Direcciones aleatorias una vez


          decmac=getRandomMac(decmac);
          printf("%s rand mac \n", decmac);
          strcpy(com6,"nmcli connection modify --temporary ");
          strcat(com6,wifi);
          strcat(com6," 802-11-wireless.cloned-mac-address ");
          strcat(com6,decmac);
          writeCmd(com6,&data);
          strcpy(com6,"nmcli connection modify --temporary ");
          strcat(com6,wifi);
          strcat(com6," ipv4.method auto");
          writeCmd(com6,&data);
          strcpy(com6,"nmcli connection up ");
          strcat(com6,wifi);
          writeCmd(com6,&data);

          break;

          case 7://test. Direcciones aleatorias cada X segundos
          if(argv[3]!=NULL){
            i=atoi(argv[3]);
          }else{
            i=10;
          }
          while(l<i){
              printf("Runing random mac every %d secs. It:%d\n",sleepTime,l );
              decmac=getRandomMac(decmac);
              printf("%s rand mac \n", decmac);
              strcpy(com6,"nmcli connection modify --temporary ");
              strcat(com6,wifi);
              strcat(com6," ipv4.method auto");
              writeCmd(com6,&data);
              strcpy(com6,"nmcli connection modify --temporary test2 802-11-wireless.cloned-mac-address ");
              strcat(com6,decmac);
              writeCmd(com6,&data);
              strcpy(com6,"nmcli connection up test2");
              writeCmd(com6,&data);
              sleep(sleepTime);
              l++;
            }

          break;

          case 8://dhcp. Direcciones aleatorias con misma IP cada x segundos. Al final la red se debe desconectar.

          if(argv[3]!=NULL){
            i=atoi(argv[3]);
          }else{
            i=10;
          }
          while(l<i){
              printf("%s%d\n","DHCP. It:",l );
              decmac=getRandomMac(decmac);
              printf("%s rand mac \n", decmac);
              strcpy(com6,"nmcli connection modify --temporary ");
              strcat(com6,wifi);
              strcat(com6," 802-11-wireless.cloned-mac-address ");
              strcat(com6,decmac);
              writeCmd(com6,&data);

              strcpy(com6,"nmcli connection modify --temporary ");
              strcat(com6,wifi);
              strcat(com6," ipv4.addresses ");
              strcat(com6,ip);
              writeCmd(com6,&data);

              strcpy(com6,"nmcli connection modify --temporary ");
              strcat(com6,wifi);
              strcat(com6," ipv4.method manual");
              writeCmd(com6,&data);

              strcpy(com6,"nmcli connection up ");
              strcat(com6,wifi);
              writeCmd(com6,&data);

              strcpy(com6,"sudo dhclient -v -cf /etc/dhcp/dhclient_tmp.conf ");
              strcat(com6,dev);
              writeCmd(com6,&data);
              sleep(sleepTime);

              strcpy(com6,"sudo dhclient -v -r ");
              strcat(com6,dev);
              writeCmd(com6,&data);
              l++;
            }

          break;

          case 9://release. Dirección aleatoria con IP especificada y usar archivo de configuración dhclient.
          decmac=getRandomMac(decmac);
          printf("%s rand mac \n", decmac);
          strcpy(com6,"nmcli connection modify --temporary test2 802-11-wireless.cloned-mac-address ");
          strcat(com6,decmac);
          writeCmd(com6,&data);
          strcpy(com6,"nmcli connection modify --temporary test2 ipv4.addresses 192.168.1.100 ");
          writeCmd(com6,&data);
          strcpy(com6,"nmcli connection modify --temporary test2 ipv4.method manual");
          writeCmd(com6,&data);
          strcpy(com6,"nmcli connection up test2");
          writeCmd(com6,&data);
          strcpy(com6,"sudo dhclient -r -v ");
          strcat(com6,dev);
          writeCmd(com6,&data);
          strcpy(com6,"sudo dhclient -v -cf /etc/dhcp/dhclient.conf ");
          strcat(com6,dev);
          printf("%s\n", com6);
          writeCmd(com6,&data);
          break;


          case 10: //clientID. Cambiar archivo de configuración desde script para que contenga la IP especificada.

          createTmp(confFile,ip);

          if(argv[3]!=NULL){
            i=atoi(argv[3]);
          }else{
            i=10;
          }
          while(l<i){
              printf("%s%d\n","DHCP tmp with client ID. It:",l );

              decmac=getRandomMac(decmac);
              printf("%s rand mac \n", decmac);
              strcpy(com6,"nmcli connection modify --temporary test2 802-11-wireless.cloned-mac-address ");
              strcat(com6,decmac);
              writeCmd(com6,&data);

              strcpy(com6,"nmcli connection modify --temporary test2 ipv4.addresses 192.168.1.100 ");
              writeCmd(com6,&data);

              strcpy(com6,"nmcli connection modify --temporary test2 ipv4.method manual");
              writeCmd(com6,&data);

              strcpy(com6,"nmcli connection up test2");
              writeCmd(com6,&data);

              strcpy(com6,"sudo dhclient -v -cf ");
              strcat(com6,confFile);
              strcat(com6,dev);
              writeCmd(com6,&data);
              printf("Sleeping %d seconds.\n", sleepTime);
              sleep(sleepTime);

              strcpy(com6,"sudo dhclient -v -r ");
              strcat(com6,dev);
              writeCmd(com6,&data);
              l++;
            }


          break;

          defaut:

          printf("%s\n", "print help");
          }



          }
          else{// si no hay modo
          printf("%s\n", "no mode");
          /*
          strcpy(com6,"sudo airmon-ng start  wlp3s0");
          writeCmd(com6,&data);
          strcpy(com6,"sudo airmon-ng stop  wlp3s0mon");
          writeCmd(com6,&data);
          strcpy(com6,"sudo airmon-ng check kill");
          writeCmd(com6,&data);
          strcpy(com6,"service network-manager start");
          writeCmd(com6,&data);


          */

          }



    }
    else{//specify a command
      printf("help\n");

    }

    free(decmac);
    free(data);
    free(com6);
    free(dev);
    //free(rMAC);
    return 0;
}
