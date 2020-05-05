#pragma once

namespace Particula {

    class HardwareStatus {
        public:
            HardwareStatus(void);

        public:
            void setTphWakeupSuccess(void);
            void setTphReadSuccess(void);
            void setParticleWakeupSuccess(void);
            void setParticleReadSuccess(void);
            void setParticleSleepSuccess(void);
            void setStat1(void);
            void setStat2(void);
            void setPg(void);
            char getStatus(void);


        private:
            char error_values = 0x00;
            /**
             * Binary coded error values
             * bit 0    Particle Sensor Wake-up Successful
             * bit 1    Particle Sensor Read Successful
             * bit 2    Particle Sensor Sleep Successful
             * bit 3    not used
             * bit 4    not used
             * bit 5    TPH Sensor Wake-up Successful
             * bit 6    TPH Sensor Read Successful
             * bit 7    not used
             * bit 8    not used
             * bit 9    not used
             * bit 10   Battery Charge Output STAT1/-LBO    // See MCP73871 Datasheet page 20 Table 5-1 for more information
             * bit 11   Battery Charge Output STAT2
             * bit 12   Battery Charge Output -PG
             * bit 13   not used
             * bit 14   not used
             * bit 15   not used
            */

        
    };

};