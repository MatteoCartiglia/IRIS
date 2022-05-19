DAC_config_settings(){
    ImGui::Begin("DAC configuration",&show_dac_config);      
    ImGui::Text("DAC configuration (mV)");      
    for(int i=0; i<MAX_DAC_CHANNEL; i++){

        std::string label = "##_";
        label += std::to_string(i);         
        const char *label_c = label.c_str();       

        std::string labelb =label +  "but";
        const char *label_b = labelb.c_str();    
        ImGui::Text("Dac number %d", i);
        ImGui::SliderInt(label_c, &DACvalue[i],0,1800);
        ImGui::SameLine();
        *&DAC_upload[i] =  ImGui::Button(label_b);

        // Upload DAC statee
        if (*&DAC_upload[i]){

            dac[i].data = (uint16_t) DACvalue[i];

            P2TPkt p2t_pk(dac[i]); 
            std::printf("Dac address, data, dac_number %d, %d, %d \n", dac[i].command_address, dac[i].data, dac[i].dac_number);
            write(serial_port, (void *) &p2t_pk, sizeof(p2t_pk));
            //std::fprintf(dac[i])
            std::printf("Dac header, body.value: %d, %d \n", p2t_pk.header, p2t_pk.body[2] << 8 | p2t_pk.body[3]);

            //int retval = read(serial_port, read_buf, sizeof(read_buf)); // in while loop? 
            // catch_retval (retval, read_buf);
            DAC_upload[i] = false;                
            }
    }
    ImGui::End();
}