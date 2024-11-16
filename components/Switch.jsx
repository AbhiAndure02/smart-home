import React, { useState } from 'react'
import { StyleSheet, Text, TouchableOpacity, View } from 'react-native'

const Switch =()=> {
    const [isEnabled, setIsEnabled] = useState(false);

    const toggleButton = () => setIsEnabled(previousState => !previousState);  
    return (
        <View style={styles.screen}>
        <Text style={styles.text}>This is the Switch screen</Text>
        <TouchableOpacity ><Text style={styles.text}>hiii sy</Text></TouchableOpacity>
        <TouchableOpacity 
        style={[styles.button, isEnabled ? styles.on : styles.off]} 
        onPress={toggleButton}
      >
        <Text style={styles.buttonText}>{isEnabled ? 'ON' : 'OFF'}</Text>
      </TouchableOpacity>
      
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
    container: {
        flex: 1,
        justifyContent: 'center',
        alignItems: 'center',
      },
      text: {
        fontSize: 20,
        marginBottom: 20,
      },
      button: {
        padding: 15,
        borderRadius: 30,
        alignItems: 'center',
      },
      on: {
        backgroundColor: 'green',
      },
      off: {
        backgroundColor: 'gray',
      },
      buttonText: {
        color: 'white',
        fontSize: 18,
      },
  });
export default Switch
