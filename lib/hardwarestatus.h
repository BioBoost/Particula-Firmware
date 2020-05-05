#pragma once

namespace Particula {

    class HardwareStatus {
        public:
            HardwareStatus(void);

        public:
            void tph_wakeup_failed(void);
            void tph_read_failed(void);
            void particle_wakeup_failed(void);
            void particle_read_failed(void);
            void particle_sleep_failed(void);
            void set_stat1(void);
            void set_stat2(void);
            void set_pg(void);
            char get_state(void);
            bool errors(void);


        private:
            // At the moment only takes into account sensors (not the battery indicator)
            char hardware_state = 0b0000000001100111;
            char successful_state = 0b0000000001100111;
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