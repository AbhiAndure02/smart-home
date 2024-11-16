import React, { Component } from 'react'
import { Text, View , StyleSheet} from 'react-native'
import Icons from 'react-native-vector-icons/FontAwesome'
import Icons2 from 'react-native-vector-icons/Entypo'

const Timer = ()=> {
  
    return (
        <View style={styles.screen}>
        <Text style={styles.text}>This is the TIMER screen</Text>
        

      </View>
    )
  
}


const styles = StyleSheet.create({
    screen: {
      flex: 1,
      justifyContent: 'center',
      alignItems: 'center',
    },
    text: {
      fontSize: 18,
    },
  });
  


export default Timer
