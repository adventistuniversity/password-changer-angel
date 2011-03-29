<?php
// $Id$
/**
* @file
* Classes to support the password changer API for Angel.
*
* Contains the definition for classes passwordChangerAngelExeption,
* passwordChangerAngelExeption and angelAPIUserManagement.
* 
*/

/**
 * Password Changer Angel exception class
 *
 * Password changer Angel exception class extends the exception class.
 *
 */
class passwordChangerAngelExeption extends Exception {}




/**
 * Represents an Angel API call. 
 *
 * Abstract class that represents the basic components of an Angel API call.
 *
 */
abstract class angelAPIIntegration {
/**
 * The Angel API user name.
 *
 * @var $username
 * 
 */   
  protected $username;
    
  /**
 * The Angel API user password.
 *
 * @var $userpwd
 * 
 */   
  protected $userpwd;
    
  /**
 * The Angel API URL to make call.
 *
 * @var $url
 * 
 */   
  protected $url;
  
/**
 * The Angel API action result.
 *
 * @var $action_result
 * 
 */   
  protected $action_result;
  
/**
 * Abstract class executeAction will execute call to an Angel API.
 *
 * @abstract executeAction. 
 * 
 * @param string $action the Angel APIACTION part of the post call to
 * the Angel service.
 * 
 * APIACTION=UPDATE_ACCOUNT&APIUSER=....
 *
 * @param $action_parameters the Angel parameters that goes with api action.
 *
 * APIACTION=UPDATE_ACCOUNT&APIUSER=apiuser&APIPASSWORD=apipassword&USER=sampleuser....
 *
 * USER would be one of the parameters for the api action UPDATE_ACCOUNT
 *  
 */ 
  abstract public function executeAction($action, $action_parameters);
  

/**
 * Construct the angelAPIIntegration.
 *
 * Set protected variables username, userpwd and url.
 * 
 * @param array $api_credentials contains the api username and password.
 *
 * @param string $api_url contains the server URL for the angel API.
 * 
 */ 
  public function __construct($api_credentials = array(), $api_url = NULL) {
    
    if (is_null($api_url)) {
            
      //Log error
      watchdog('password_changer_angel', 'Angel API URL not set.', WATCHDOG_ERROR);
      //Throw exception
       throw new passwordChangerAngelExeption('Angel API URL not set.');
        
    }
        
    //Set private data
    $this->username = (isset($api_credentials['api_username'])) ? $api_credentials['api_username'] : '' ;
    $this->userpwd = (isset($api_credentials['api_password']))? $api_credentials['api_password']:'';
    $this->url = $api_url;
     
  }
    
}
  
/**
 * Represents an Angel API call to manage a users.
 *
 * Make Angel API call to manage users and extends angelAPIIntegration.
 *
 */  
  
class angelAPIUserManagement extends angelAPIIntegration {
    
  /**
  * Represents an Angel API call to manage a users.
  *
  * Make Angel API call to manage users and extends angelAPIIntegration.
  *
  * @param string $action api action CHECK_ACCOUNT.
  *
  * @param array $action_paramenters api action parameters that is needed for the
  * call.
  * 
  * APIACTION=UPDATE_ACCOUNT&APIUSER=apiuser&APIPASSWORD=apipassword&USER=sampleuser....
  * 
  */   
   public function executeAction($action, $action_paramenters) {
    
      try {
        //Check parameters
        $paramenters = 'APIACTION=' . $action . '&APIUSER=' . $this->username . '&APIPWD=' . $this->userpwd;
    
        //Add value to the action parameters
        foreach ($action_paramenters AS $property => $value) {
          //Add values parameters
          $paramenters .= '&' . $property . '=' . $value;      
        }
        
        //call cURL execute and send the user
        $header = 'Content-type: application/x-www-form-urlencoded';
    
        //Set cURL options
        $options = array(CURLOPT_POST => 1,
                      CURLOPT_HTTPHEADER => array( $header ),
                      CURLOPT_POSTFIELDS => $paramenters,
                      CURLOPT_RETURNTRANSFER => 1,
                      CURLOPT_SSL_VERIFYPEER => 0,
                  );
         //Set action results
         $this->action_result = password_changer_api_simple_curl_call($this->url, $options);
     
         //Make cURL action using helper function from API and set it to action
         return $this->action_result;
     }
     catch (passwordChangerApiExeption $e) {
      
       watchdog('password_changer_angel', $e->getMessage());   
      
       throw new passwordChangerAngelExeption($e->getMessage());
      
     } 
  }
}
/**
 * Get the SONIS.
 *
 * Given an opid (user name in drupal sites) return the SONIS id for the user.
 *
 * @param string $opid the user opid
 *
 * @return SONIS id for the user
 *
 */

function password_changer_angel_get_sonis_id($opid) {
  //Check if $opid is of lenght 6
  if(strlen($opid) == 6) {
  
    //Make sonis call to userExist in SONIS
    $results = sonisweb_integration_api("SonisWeb200.CustomPages.password_changer", "getSonisId", "yes",
                                       array(array('opid', $opid)));

    //Check $result is equal to FALSE if so return FALSE
    if($results == 'False') {
        
        return FALSE;
    }
    //Return $results           
    return $results;
  }
 
} 