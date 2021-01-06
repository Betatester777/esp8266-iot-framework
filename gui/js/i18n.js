import React from "react";
import de_de from "./i18n/de-de.json";

function reEscape(s) {
  return s.replace(/([.*+?^$|(){}\[\]])/mg, "\\$1");
}

export default class i18n{
  constructor(){
    this.locales=Object.freeze({
        "de-de": de_de
    });

    this.currentLocale="de-de";
  }

  setLocale=(locale)=>{
    if(this.locales.hasOwnProperty(locale)){
      this.currentLocale=locale;
    }
  }

  get=(key, customExpressions)=>{
    let text="#NOT_FOUND#";

    if(this.locales[this.currentLocale].hasOwnProperty(key)){
      text=this.locales[this.currentLocale][key];
      let expressions=[
        ["{br}", "<br/>"]
      ];

      expressions.map((expression)=>{
        if(expression[0]){
          let regEx = new RegExp(reEscape(expression[0]),'g');
          text=text.replace(regEx, expression[1]);
        }
      });

      if(customExpressions){
        customExpressions.map((expression)=>{
          if(expression[0]){
            let regEx = new RegExp(reEscape(expression[0]),'g');
            text=text.replace(regEx, expression[1]);
          }
        });
      }
    }

    return <span dangerouslySetInnerHTML={{__html : text}} />;
  }
};
