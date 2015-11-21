         RPC_C_IMP_LEVEL_IMPERSONATE_BIPC,
            0,
            EOAC_NONE_BIPC
            )
         ){
         return false;
      }

      com_releaser<IWbemServices_BIPC> IWbemServices_releaser(pWbemServices);

      strValue.clear();
      strValue += L"Select ";
      strValue += wmi_class_var;
      strValue += L" from ";
      strValue += wmi_class;

      IEnumWbemClassObject_BIPC * pEnumObject  = 0;

      if ( 0 != pWbemServices->ExecQuery(
            (bstr)L"WQL",
            (bstr)strValue.c_str(),
            //WBEM_FLAG_RETURN_IMMEDIATELY_BIPC,
            WBEM_FLAG_RETURN_WHEN_COMPLETE_BIPC | WBEM_FLAG_FORWARD_ONLY_BIPC,
            0,
            &pEnumObject
            )
         ){
         return false;
      }

      com_releaser<IEnumWbemCla